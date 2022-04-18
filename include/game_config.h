// Config variables for Snake game

// joystick threshold (reminder: value is 0..1023)
#define LOWER ((1024/2)-200)
#define UPPER ((1024/2)+200)

// initial snake values
#define INIT_HEAD_X 3
#define INIT_HEAD_Y 1
#define INIT_LEN 3
#define INIT_DIR RIGHT

// initial food position (ignore if random init)
#define INIT_FOOD_X 4
#define INIT_FOOD_Y 4

// display settings
// time in ms between each frame update
#define FRAME_REFRESH_TIME 400
// time in ms between each food blink (period/2)
#define FOOD_BLINK_TIME 150
// led matrix brightness (0..15)
#define INTENSITY 4
