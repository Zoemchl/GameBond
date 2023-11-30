#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>

#include <esp_now.h>
#include <WiFi.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define SDA_PIN 5
#define SCL_PIN 6

// Définir les pins GPIO pour les boutons
#define O_1 20 // GPIO pour Output 1
#define O_2 10 // GPIO pour Output 2
#define O_3 0  // GPIO pour Output 3
#define I_1 7  // GPIO pour Input 1
#define I_2 8  // GPIO pour Input 2
#define I_3 9  // GPIO pour Input 3


const unsigned char GUN_BITMAP[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x03, 0x80, 0x01, 0x20, 0x07, 0xff, 0xfd, 0x00, 
	0x1c, 0x30, 0x04, 0xe8, 0x09, 0x00, 0x1c, 0x00, 0x1d, 0x40, 0x0d, 0x20, 0x07, 0xff, 0xf9, 0x48, 
	0x07, 0x76, 0x00, 0x10, 0x05, 0xe4, 0x02, 0xc0, 0x07, 0xec, 0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 
	0x0d, 0xc0, 0x00, 0x00, 0x1f, 0x80, 0x00, 0x00, 0x1b, 0x80, 0x00, 0x00, 0x1f, 0x80, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char SHIELD_BITMAP [] PROGMEM = {
	0x00, 0x7f, 0xfc, 0x00, 0x01, 0xff, 0xff, 0x00, 0x03, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0x80, 
	0x03, 0xc0, 0x07, 0x80, 0x03, 0xc0, 0x07, 0x80, 0x03, 0xc0, 0x07, 0x80, 0x03, 0xc0, 0x07, 0x80, 
	0x03, 0xc0, 0x07, 0x80, 0x03, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0x80, 
	0x03, 0xd4, 0x47, 0x80, 0x03, 0xc6, 0xdf, 0x80, 0x03, 0xd6, 0xc7, 0x80, 0x03, 0xd6, 0xd7, 0x80, 
	0x03, 0xd4, 0x47, 0x80, 0x03, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0x80, 
	0x03, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0x80, 
	0x03, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0x80, 
	0x03, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0x80, 0x01, 0xff, 0xff, 0x00, 0x00, 0x7f, 0xfc, 0x00
};
const unsigned char RELOAD_BITMAP [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x01, 0xc1, 0xc1, 0xc0, 
	0x01, 0xc1, 0xc1, 0xc0, 0x01, 0xc1, 0xc1, 0xc0, 0x03, 0xe3, 0xe3, 0xe0, 0x03, 0xe3, 0xe3, 0xe0, 
	0x03, 0xe3, 0xe3, 0xe0, 0x03, 0xe3, 0xe3, 0xe0, 0x02, 0x22, 0x22, 0x20, 0x03, 0xe3, 0xe3, 0xe0, 
	0x03, 0xe3, 0xe3, 0xe0, 0x07, 0xf7, 0xf7, 0xf0, 0x06, 0xf6, 0xf6, 0xf0, 0x06, 0xf6, 0xf6, 0xf0, 
	0x06, 0xf6, 0xf6, 0xf0, 0x06, 0xf6, 0xf6, 0xf0, 0x06, 0xf6, 0xf6, 0xf0, 0x06, 0xf6, 0xf6, 0xf0, 
	0x06, 0xf6, 0xf6, 0xf0, 0x06, 0xf6, 0xf6, 0xf0, 0x06, 0xf6, 0xf6, 0xf0, 0x06, 0xf6, 0xf6, 0xf0, 
	0x06, 0xf6, 0xf6, 0xf0, 0x06, 0xf6, 0xf6, 0xf0, 0x07, 0xf7, 0xf7, 0xf0, 0x07, 0xf7, 0xf7, 0xf0, 
	0x03, 0xe3, 0xe3, 0xe0, 0x03, 0xe3, 0xe3, 0xe0, 0x04, 0x14, 0x14, 0x10, 0x1f, 0xff, 0xff, 0xfc
};

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const byte ROWS = 3; // trois rangées
const byte COLS = 3; // trois colonnes

// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

typedef struct struct_message {

} struct_message;


// Définir les broches de connexion du clavier
byte rowPins[ROWS] = {O_1, O_2, O_3}; // Connecter à O_1
byte colPins[COLS] = {I_1, I_2, I_3}; // Connecter à I_1, I_2, I_3

// Définir les boutons du clavier
char keys[ROWS][COLS] = {
  {'U', 'L', 'D'},
  {'R', 'S', 'T'},
  {'A', 'B', 'Y'}
};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int x = SCREEN_WIDTH / 2;
int y = SCREEN_HEIGHT / 2;
int ya = SCREEN_HEIGHT;
int xa = SCREEN_WIDTH;
int radius = 5;

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  
  updateDisplay();
}

// Add to your existing variables
int bulletsPlayer1 = 0;
int bulletsPlayer2 = 0;
int scorePlayer1 = 0;
int scorePlayer2 = 0;
bool gameStarted = false;
enum PlayerAction { RECHARGE, PROTECT, SHOOT };
const char* actionNames[] = {"Recharge", "Protect", "Shoot"};
int currentSelection = 0;
bool actionSelectScreen = false;
unsigned long decisionStartTime = 0;
const unsigned long decisionDuration = 5000; // 5000 milliseconds = 5 seconds
bool isTimerActive = false;
int remainingTime = 5; // Remaining time in seconds
bool loadingRectMovingRight = true;
int loadingRectStep = 0;
int opponentMove = 0;
int selectedMove = 0;
bool resultScreen = false;


void loop() {
  char key = keypad.getKey();
  if (!gameStarted) {
    drawWaitingScreen();
  }
  
  if (actionSelectScreen) {
    // Player 1 controls
    if (!isTimerActive) {
      decisionStartTime = millis();
      isTimerActive = true;
      remainingTime = 5; // Reset to 5 seconds
    }

    // Update remaining time every second
    long elapsedTime = millis() - decisionStartTime;
    if (elapsedTime >= 1000) { // 1 second has passed
      remainingTime = 5 - elapsedTime / 1000;
      decisionStartTime = millis(); // Reset start time for next second
    }

    // Check if 5 seconds have passed
    if (remainingTime <= 0) {
      // Time is up, process the decision or move to the next step
      isTimerActive = false; // Stop the timer
      actionSelectScreen = false;
      handleSelection();
    }

    if (key) {
      // Previous action in the list if left and/or up is pressed
      if (key == 'L' || key == 'U') { 
        currentSelection = (currentSelection + 2) % 3; // Wrap around backwards
      }
      if (key == 'R' || key == 'D') { 
        currentSelection = (currentSelection + 1) % 3; // Wrap around forwards
      }
    }
    if (resultScreen) {
      if (!isTimerActive) {
      decisionStartTime = millis();
      isTimerActive = true;
      remainingTime = 5; // Reset to 5 seconds
      }

      // Update remaining time every second
      long elapsedTime = millis() - decisionStartTime;
      if (elapsedTime >= 1000) { // 1 second has passed
        remainingTime = 5 - elapsedTime / 1000;
        decisionStartTime = millis(); // Reset start time for next second
      }

      // Check if 5 seconds have passed
      if (remainingTime <= 0) {
        // Time is up, process the decision or move to the next step
        isTimerActive = false; // Stop the timer
        actionSelectScreen = true;
        resultScreen = false;
      }
    }
    updateDisplay();
  }
}

void resolveRound(int playerAction, int opponentAction) {
    bool playerCanShoot = playerAction == SHOOT && bulletsPlayer1 > 0;
    bool opponentCanShoot = opponentAction == SHOOT && bulletsPlayer2 > 0;

    switch (playerAction) {
        case RECHARGE:
            bulletsPlayer1++;
            break;
        case SHOOT:
            if (playerCanShoot) bulletsPlayer1--;
            break;
    }

    switch (opponentAction) {
        case RECHARGE:
            bulletsPlayer2++;
            break;
        case SHOOT:
            if (opponentCanShoot) bulletsPlayer2--;
            break;
    }


    if (playerCanShoot && opponentAction == RECHARGE) {
        scorePlayer1++; // Player hits the recharging opponent
    } else if (opponentCanShoot && playerAction == RECHARGE) {
        scorePlayer2++; // Opponent hits the recharging player
    }

}


void handleSelection() {
  // SEND MY SELECTION TO OPPONENT
  selectedMove = currentSelection;
  // RECEIVE THE OPPONENT SELECTION
  // While true  {
  // wait for response from opponent
  //opponentMove = ???;
  //}
  resolveRound(selectedMove, opponentMove);
  resultScreen = true;
}




void drawWaitingScreen() {
  // Set text size and color
  display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(16, 26);
  display.print("Waiting for players");

}

void drawResultScreen() {
  int bitmapHeight = 32;
  int spacing = 8;
  int yPosition = 13;
  int x1 = 29;
  int x2 = 61;
  int ypos = 13;
  uint16_t bitmapColor = SSD1306_WHITE;
  display.setTextSize(1);      
    display.setTextColor(SSD1306_WHITE);



  display.setCursor(14, 13);
  display.print("You");
  switch (selectedMove) {
    case RECHARGE:
        display.drawBitmap(x1, ypos, RELOAD_BITMAP, bitmapHeight, bitmapHeight, bitmapColor);
        break;
    case PROTECT:
        display.drawBitmap(x1, ypos, SHIELD_BITMAP, bitmapHeight, bitmapHeight, bitmapColor);
        break;
    case SHOOT:
        display.drawBitmap(x1, ypos, GUN_BITMAP, bitmapHeight, bitmapHeight, bitmapColor);
        break;
  }
  switch (opponentMove) {
    case RECHARGE:
        display.drawBitmap(x2, ypos, RELOAD_BITMAP, bitmapHeight, bitmapHeight, bitmapColor);
        break;
    case PROTECT:
        display.drawBitmap(x2, ypos, SHIELD_BITMAP, bitmapHeight, bitmapHeight, bitmapColor);
        break;
    case SHOOT:
        display.drawBitmap(x2, ypos, GUN_BITMAP, bitmapHeight, bitmapHeight, bitmapColor);
        break;
  }
  display.setCursor(93, 13);
  display.print("Them");
}

void drawActionSelectScreen() {

    // Define the height of each bitmap and the spacing
    int bitmapHeight = 32;
    int spacing = 8;
    int yPosition = 13;

    for (int i = 0; i <= SHOOT; i++) {
        int xposition = i * 40 + 8;

        // Determine the color of the bitmap based on selection
        uint16_t bitmapColor = (i == currentSelection) ? SSD1306_BLACK : SSD1306_WHITE;

        // Highlight the selected action
        if (i == currentSelection) {
            display.fillRect(xposition, yPosition, bitmapHeight, bitmapHeight, SSD1306_WHITE);
        }

        // Display the corresponding bitmap
        switch (i) {
            case RECHARGE:
                display.drawBitmap(xposition, yPosition, RELOAD_BITMAP, 32, 32, bitmapColor);
                break;
            case PROTECT:
                display.drawBitmap(xposition, yPosition, SHIELD_BITMAP, 32, 32, bitmapColor);
                break;
            case SHOOT:
                display.drawBitmap(xposition, yPosition, GUN_BITMAP, 32, 32, bitmapColor);
                break;
        }
    }
}
void drawHUD() {
     // Set text size and color
    display.setTextSize(1);      
    display.setTextColor(SSD1306_WHITE);

    // Prepare the score string and calculate its length
    String scoreStr = String(scorePlayer1) + " - " + String(scorePlayer2);
    int scoreStrLength = 6 * scoreStr.length(); // Approximate length: 6 pixels per character

    // Display the combined score in the center of the top section
    int scoreXPosition = (SCREEN_WIDTH - scoreStrLength) / 2;
    display.setCursor(scoreXPosition, 0); // Dynamically calculate to center the score
    display.print(scoreStr);

    // Display "You" just to the left of the score
    display.setCursor(scoreXPosition - 6 * 4, 0); // "You" is 3 characters plus a space
    display.print("You ");

    // Display "Opponent" just to the right of the score
    display.setCursor(scoreXPosition + scoreStrLength, 0);
    display.print(" Them");

    // Draw a horizontal line to separate the score from the main area
    display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

    // Display Player 1 ammo count at the bottom left edge
    display.setCursor(0, 54); // Adjust y as needed
    display.print("Ammo: ");
    display.println(bulletsPlayer1);

    // Display Player 2 ammo count at the bottom right edge
    int ammoStrLength = 6 * String(bulletsPlayer2).length(); // Calculate length of ammo string
    display.setCursor(SCREEN_WIDTH - ammoStrLength - 6 * 6, 54); // Adjust for the length of "Ammo: " text
    display.print("Ammo: ");
    display.println(bulletsPlayer2);


    // Draw a horizontal line to separate main area from bullet counts
    display.drawLine(0, 48, 128, 48, SSD1306_WHITE);
}


void updateDisplay() {
    display.clearDisplay();
    if (actionSelectScreen) {
      drawActionSelectScreen();
    }
    drawHUD();
    display.display();
}




