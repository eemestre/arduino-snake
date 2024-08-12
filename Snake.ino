// snake struct
typedef struct {
  int MAX_BODY_SIZE;
  int size;
  bool alive;
  bool won;
  bool justDied;
  bool justWon;
  int body[63][2];
  int x;
  int y;
  int direction;
} Snake;

// food struct
typedef struct {
  bool eaten ;
  bool placed;
  int x;
  int y;
} Food;

// pinout
int latchPin = 5;
int clockPin = 6;
int dataPin = 4;
int rows[8] = {2, 3, 7, 8, 9, 10, 11, 12};
int joystick_x_pin = A5;
int joystick_y_pin = A4;
int buttonValue_pin = A3;

// grids
int grid[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 0}};

int snakeGrid[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0},
                             {0, 0, 0, 0, 0, 0, 0, 0},
                             {0, 0, 0, 0, 0, 0, 0, 0},
                             {0, 0, 0, 0, 0, 0, 0, 0},
                             {0, 0, 0, 0, 0, 0, 0, 0},
                             {0, 0, 0, 0, 0, 0, 0, 0},
                             {0, 0, 0, 0, 0, 0, 0, 0},
                             {0, 0, 0, 0, 0, 0, 0, 0}};
                             
int foodGrid[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0}};

int skull[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0},
                   {0, 0, 1, 1, 1, 1, 0, 0},
                   {0, 1, 1, 1, 1, 1, 1, 0},
                   {0, 1, 0, 1, 1, 0, 1, 0},
                   {0, 1, 0, 1, 1, 0, 1, 0},
                   {0, 0, 1, 1, 1, 1, 0, 0},
                   {0, 0, 1, 0, 0, 1, 0, 0},
                   {0, 0, 0, 1, 1, 0, 0, 0}};

int star[8][8] = {{0, 0, 0, 1, 1, 0, 0, 0},
                  {0, 0, 1, 0, 0, 1, 0, 0},
                  {1, 1, 1, 0, 0, 1, 1, 1},
                  {1, 0, 0, 0, 0, 0, 0, 1},
                  {0, 1, 0, 0, 0, 0, 1, 0},
                  {1, 0, 0, 0, 0, 0, 0, 1},
                  {1, 0, 0, 1, 1, 0, 0, 1},
                  {1, 1, 1, 0, 0, 1, 1, 1}};

// general control variables
unsigned long old = 0;
unsigned long now = 0;
int inputs[3] = {-1, -1, -1}; // {x direction, y direction, button pressed (1 or 0)}
int tps = 4;
int fps = 120;
int aux = 0;
int period = 100;
int counter = 0;
byte data = 0;

// snake and food creating
Snake* snake = (Snake*) malloc(sizeof(Snake));
Food* food = (Food*) malloc(sizeof(Food));

void setup() {
  Serial.begin(300);

  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);

  for(int i = 0; i < 8; i++) {
    pinMode(rows[i], OUTPUT);
    digitalWrite(rows[i], HIGH);
  }

  newSnake();
  newFood();
}

// game loop
void loop() {
  now = millis();
  getInputs();

  if(now - old >= 1000/tps) {
    tick();
    old = now;
  }

  render();
}

void newSnake() {
  snake->MAX_BODY_SIZE = 30;
  snake->size = 1;
  snake->alive = true;
  snake->won = false;
  snake->justDied = false;
  snake->justWon = false;
  snake->body[0][0] = 3;
  snake->body[0][1] = 2;
  for(int i = 1; i < 63; i++) {
    for(int j = 0; j < 2; j++) {
      snake->body[i][j] = -1;
    }
  }
  snake->x = 3;
  snake->y = 3;
  snake->direction = 1;  // up = 0; down = 1; right = 2; left = 3
}

void newFood() {
  food->eaten = false;
  food->placed = false;
  
  while(food->placed == false) {
    food->x = random(0, 8);
    food->y = random(0, 8);
    food->placed = true;

    for(int i = 0; i < snake->size; i++) {
      if(food->x == snake->body[i][0] && food->y == snake->body[i][1]) {
        food->placed = false;
        break;
      }
    }

    if(food->x == snake->x && food->y == snake->y) {
      food->placed = false;
    }
  }
}

void snakeTick() {
  // update snake direction
  if(inputs[0] != -1) {
    if(snake->direction == 2 && inputs[0] == 3) {}
    else if(snake->direction == 3 && inputs[0] == 2) {}
    else {
      snake->direction = inputs[0];
    }
  } else if(inputs[1] != -1) {
    if(snake->direction == 0 && inputs[1] == 1) {}
    else if(snake->direction == 1 && inputs[1] == 0) {}
    else {
      snake->direction = inputs[1];
    }
  }

  // save snake head x and y to update body
  int aux_x, aux_y;
  aux_x = snake->x;
  aux_y = snake->y;

  // update snake head position
  if(snake->direction == 1) {
    if(snake->y == 7) {
      snake->y = 0;
    } else {
      snake->y++;
    }
  } else if(snake->direction == 0) {
    if(snake->y == 0) {
      snake->y = 7;
    } else {
      snake->y--;
    }
  } else if(snake->direction == 2) {
    if(snake->x == 7) {
      snake->x = 0;
    } else {
      snake->x++;
    }
  } else if(snake->direction == 3) {
    if(snake->x == 0) {
      snake->x = 7;
    } else {
      snake->x--;
    }
  }

  // check if ate food
  if(snake->x == food->x && snake->y == food->y) {
    food->eaten = true;
    snake->body[snake->size][0] = 0;
    snake->body[snake->size][1] = 0;
    snake->size++;
  }

  // update snake body
  for(int i = snake->size - 1; i > 0; i--) {
    snake->body[i][0] = snake->body[i-1][0];
    snake->body[i][1] = snake->body[i-1][1];
  }
  snake->body[0][0] = aux_x;
  snake->body[0][1] = aux_y;

  // check if died
  for(int i = 0; i < snake->size; i++) {
    if(snake->x == snake->body[i][0] && snake->y == snake->body[i][1]) {
      snake->alive = false;
      snake->justDied = true;
      break;
    }
  }

  // check if won
  if(snake->size == snake->MAX_BODY_SIZE - 1) {
    snake->won = true;
    snake->justWon = true;
  }
}

void snakeRender() {
  int aux_x, aux_y;

  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++){
      snakeGrid[i][j] = 0;
    }
  }

  snakeGrid[snake->y][snake->x] = 1;

  for(int i = 0; i < snake->size; i++) {
    aux_x = snake->body[i][0];
    aux_y = snake->body[i][1];
    snakeGrid[aux_y][aux_x] = 1;
  }
}

void foodRender() {
  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++){
      foodGrid[i][j] = 0;
    }
  }

  foodGrid[food->y][food->x] = 1;
}

void getInputs() {
  inputs[0] = -1;
  inputs[1] = -1;
  inputs[2] = -1;
  int bx = analogRead(joystick_x_pin);
  int by = analogRead(joystick_y_pin);
  int b = analogRead(buttonValue_pin);

  if(b < 300) {
    inputs[2] = 1;
  } else {
    inputs[2] = 0;
  }

  if(bx < 100) {
    inputs[0] = 3;
  } else if(bx > 900){
    inputs[0] = 2;
  }

  if(by < 100) {
    inputs[1] = 0;
  } else if(by > 900){
    inputs[1] = 1;
  }
}

void deathScreen() {
  if(snake->justDied) {
    for(int i = 0; i < 8; i++) {
      for(int j = 0; j < 8; j++) {
        grid[i][j] = skull[i][j];
      }
    }
    snake->justDied = false;
  }

  if(counter < 7) {
    if(aux > 1) {
      counter++;
      aux = 0;
    } else {
      aux++;
    }
  } else {
    counter = 0;
    newSnake();
  }
}

void winScreen() {
  if(snake->justWon) {
    for(int i = 0; i < 8; i++) {
      for(int j = 0; j < 8; j++) {
        grid[i][j] = star[i][j];
      }
    }
    snake->justWon = false;
  }

  if(counter < 7) {
    if(aux > 1) {
      counter++;
      aux = 0;
    } else {
      aux++;
    }
  } else {
    counter = 0;
    newSnake();
  }
}

void sendData() {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, data);
  digitalWrite(latchPin, HIGH);
}

void tick() {
  if(snake->alive) {
    if(snake->won) {
      winScreen();
    } else {
      snakeTick();
      if(food->eaten) {
        newFood();
      }
    }
  } else {
    deathScreen();
  }
}

void render() {
  if(snake->alive && !snake->won) {
    snakeRender();
    foodRender();

    for(int i = 0; i < 8; i++) {
      for(int j = 0; j < 8; j++) {
        grid[i][j] = snakeGrid[i][j] + foodGrid[i][j];
      }
    }
  }

  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++) {
      if(grid[i][j] == 1) {
        bitSet(data, j);
      } else {
        bitClear(data, j);
      }
    }

    sendData();
    digitalWrite(rows[i], LOW);
    delayMicroseconds(period);
    digitalWrite(rows[i], HIGH);
  }
}