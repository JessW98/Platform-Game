#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>
#include <time.h>

//Settings
#define BLOCK_WIDTH (9)
#define BLOCK_HEIGHT (2)
#define MAX_SAFE_BLOCKS (160)
#define MAX_UNSAFE_BLOCKS (40)
#define KNIGHT_HEIGHT (6)
#define KNIGHT_WIDTH (10)
#define HEART_HEIGHT (5)
#define HEART_WIDTH (8)

#define DELAY (30)

//Bool arguments to control if game is over, if heart moves and what direction it moves in
bool game_over = false;
bool heart_move= true;
bool heart_right= true;
bool gravity_acting= true;
//Images of player and blocks
char *fall_knight_image =
    " \\( ) /   "
    "  \\| /   "
    "    |     "
    "    |     "
    "   / \\    "
    " _/   \\_   ";

char *left_knight_image=
    "   ( )    "
    "  |_|_   /"
    "    | |_/ "
    "    |  /  "
    "  --|     "
    "_|   \\_   ";

char *right_knight_image=
    "     ( )    "
    "\\  _|_ /   "
    "\\| |      "
    " \\ |      "
    "   |--     "
    "_/  |_   ";


char *knight_image =
    "   ( )    "
    "   /|\\   /"
    "  / | \\ / "
    "    |  /   "
    "  / \\    "
    " _/   \\_   ";

char *spawn_knight_1_image =
    "* *( )* * "
    " * /|\\** /"
    "* / | \\*/*"
    "* * |**/** " 
    " */ \\**  "
    "*_/* *\\_* *";
char *spawn_knight_image=
    "**********"
    "**********"
    "**********"
    "**********"
    "**********"
    "**********";

char *safe_blocks_image =
    " ======= "
    " ======= ";

char *unsafe_blocks_image =
    " xxxxxxx "
    " xxxxxxx ";

char *heart_image =
    " + ++ + "
    "(  \\/  )"
    " \\    / "
    "+ \\  /  "
    " ++\\/++ +";

char *heart_image_2 =
    "+ +  + +"
    "(  \\/  )"
    "+\\    /+"
    " +\\  /++"
    "+  \\/  + ";

char *broken_heart_image =
    "        "
    "(  \\/  )"
    " \\  \\ / "
    "  \\ // "
    "    \\/   ";

char *student_number =
    " n10194363 ";

// declare sprite IDs
sprite_id knight;
sprite_id heart;
Sprite safe_blocks[MAX_SAFE_BLOCKS];
Sprite unsafe_blocks[MAX_UNSAFE_BLOCKS];

//counters for safe, unsafe block and lives
int safe_ctr =1;
int unsafe_ctr=1;
int lives =10;
int score= 0;

//timer global variable
int StartTime;

//Array for blocks that have been stepped on
Sprite stepped_on[1] = {};

//create a safe block sprite template 
void safe_template(Sprite *safe_blocks)
{
    safe_blocks->x = 0;
    safe_blocks->y = 0;
    safe_blocks->width = 9;
    safe_blocks->height = 2;
    safe_blocks->bitmap =safe_blocks_image;
    safe_blocks->is_visible =true;

}


//create an unsafe block sprite template
void unsafe_template(Sprite *unsafe_blocks)
{
    unsafe_blocks->x = 0;
    unsafe_blocks->y = 0;
    unsafe_blocks->width = 9;
    unsafe_blocks->height = 2;
    unsafe_blocks->bitmap = unsafe_blocks_image;
    unsafe_blocks->is_visible =true;

}

//timer
void timer(){
    // calculate time
    int end = get_current_time();
    int time_spent = end -StartTime;
    int seconds = time_spent % 60;
    int minutes = time_spent / 60;
    //draw time
    draw_string((screen_width() / 5) * 3, 2.5, "timer:");
    draw_int(((screen_width() / 5) * 3)+7, 2.5, minutes);
    draw_string(((screen_width() / 5) * 3)+9, 2.5, "m:");
    draw_int(((screen_width() / 5) * 3)+13, 2.5, seconds);
    draw_string(((screen_width() / 5) * 3)+15, 2.5, "s");
}

//draw display box
void draw_display_box(void)
{
    // lines
    draw_line(0, 0, screen_width() - 1, 0, '~');
    draw_line(0, 4,screen_width() - 1, 4, '~');

    //Student Number
    draw_string(screen_width() / 5, 2.5, student_number);
    //lives
    draw_string((screen_width() / 5) * 2, 2.5 , "lives:");
    draw_int((screen_width() / 5) * 2+7, 2.5, lives);
    //score
    draw_string((screen_width() / 5) * 4, 2.5, "Score:");
    draw_int((screen_width() / 5) * 4+7, 2.5, score);


    //draw timer
    timer();
}
//draw blocks
void draw_blocks()
{
    //draw the safe blocks
    for (int i = 0; i <= safe_ctr; i++)
    {
        sprite_draw(&safe_blocks[i]);
    }
    //draw unsafe blocks
    for (int x = 0; x <= unsafe_ctr; x++)
    {
        sprite_draw(&unsafe_blocks[x]);
    }
    show_screen();
}

//create blocks
void create_blocks()
{
    //use the time as a seeed for random
    srand(time(NULL));
    // number of columns is the length of the screen divided by the width of blocks, calculates how many blocks can fit in the screen.
    int columns = screen_width() / BLOCK_WIDTH;
    //number of rows determined by the screen height divided by the block height and the knight
    int rows = screen_height() / ((BLOCK_HEIGHT + KNIGHT_HEIGHT + 2));
    //for each row, starting from one, ending at the number of rows that can fit into screen
    for (int r = 1; r < rows-1; r++)
    {
        // the y position of the block is the display screen length plus the height of the knight plus two for a buffer zone, multiplied by the number of rows.
        int y = screen_height() / 8 + (BLOCK_HEIGHT + (KNIGHT_HEIGHT + 2+2) * r);
        // for each column
        for (int c = 0; c < columns; c++)
        {
            // dx is a random column
            int x = ((rand() % (columns + 1 - 0)) * BLOCK_WIDTH);
            //if SafeOrNot ends up being over 50, a safe block will be drawn, else it will be an unsafe block. there is a 1/5 chance of it being unsafe.
            int safeOrNot = rand() % 50;
            if (safeOrNot > 10)
            {
                safe_template(&safe_blocks[safe_ctr]);
                safe_blocks[safe_ctr].x =x;
                safe_blocks[safe_ctr].y =y;
                safe_ctr++;
            }
            else
            {
                unsafe_template(&unsafe_blocks[unsafe_ctr]);
                unsafe_blocks[unsafe_ctr].x =x;
                unsafe_blocks[unsafe_ctr].y =y;
                unsafe_ctr++;
            }
        }
    }
}
void update_draw();
// function to return knight to start
void return_knight(void){
    int columns = screen_width() / BLOCK_WIDTH;
    int dx = ((rand() % (columns + 1 - 0)) * BLOCK_WIDTH);
    int dy = ((screen_height() / 8 + (BLOCK_HEIGHT + 11)) - KNIGHT_HEIGHT - 3);
    sprite_move_to(knight, dx,dy);
    sprite_set_image(knight, spawn_knight_image);
    update_draw();
    timer_pause(300);
    sprite_set_image(knight, spawn_knight_1_image);
    update_draw();
    timer_pause(400);
    sprite_set_image(knight, knight_image);
}

//create knight
void create_knight(void)
{
    //use the time as a seeed for random
    srand(time(NULL));
    // random x position
    int columns = screen_width() / BLOCK_WIDTH;
    int dx = ((rand() % (columns + 1 - 0)) * BLOCK_WIDTH);
    int dy = ((screen_height() / 8 + (BLOCK_HEIGHT + 11)) - KNIGHT_HEIGHT - 3);
    //create and draw sprite
    knight = sprite_create(dx, dy, KNIGHT_WIDTH, KNIGHT_HEIGHT, knight_image);
}

//create treasure 
void create_treasure(void)
{
    heart= sprite_create(screen_width()/2, screen_height()-HEART_HEIGHT, HEART_WIDTH, HEART_HEIGHT, heart_image);
}

//update the screen
void update_draw()
{
    clear_screen();
    sprite_draw(knight);
    sprite_draw(heart);
    draw_display_box();
    draw_blocks();
    show_screen();
}

//movement control
// void jump(void){
//     sprite_move(knight,0,-2);  
//     sprite_set_image(knight, jump_knight_image);  
//     timer_pause(5);
//     sprite_set_image(knight, knight_image);
// }

void move_knight(int Key)
{
    
    if (Key == 'a'&& sprite_x(knight) > 0)
    {
        sprite_set_image(knight, left_knight_image);
        sprite_move(knight, -1, 0);
    }
    else if (Key == 'd' && sprite_x(knight) < screen_width()-KNIGHT_WIDTH)
    {
        sprite_set_image(knight, right_knight_image);
        sprite_move(knight, +1, 0);
    }
    else
    {
        sprite_set_image(knight, knight_image);
    }
}
void toggle_heart(int Key){
    if (Key == 't'){
        heart_move = !heart_move;
    }

}

void move_heart(){
    
    if(heart->bitmap == heart_image){
        sprite_set_image(heart, heart_image_2);
    }else{
        sprite_set_image(heart, heart_image);
    }


    if (sprite_x(heart)==1){
        heart_right =true;
    }
    if (sprite_x(heart)== screen_width()-HEART_WIDTH){
        heart_right =false;
    }

    if (heart_right == true){
        sprite_move(heart, +0.5,0);

    }else{
        sprite_move(heart, -0.5,0);
    }
    
}

// gravity and falling
bool gravity(void){
    
    //check if knight is on a safe block
    for (int i = 0; i <= safe_ctr; i++)
    {
        int left_x_knight = round(sprite_x(knight));
        int left_x_block = round(safe_blocks[i].x);
        int right_x_knight = left_x_knight + sprite_width(knight) - 1;
        int right_x_block = left_x_block + BLOCK_WIDTH - 1;
        int knight_y = sprite_y(knight) + sprite_height(knight)+1;
        int block_y = safe_blocks[i].y;

        // if either leg is on a platform (true) and the y position of knight is -1 of block then gravity = false
        if((knight_y == block_y+1) && (left_x_knight < right_x_block) && (right_x_knight > left_x_block)){
            return false;
        }
        if(knight_y >= screen_height() + KNIGHT_HEIGHT){
            return_knight();
            lives -= 1;
            return false;
        }
    }
    return true;
        
}
void fall(void){
    sprite_move(knight, 0, +1);
}

// checking for collision with unsafe block
void check_unsafe_block()
{
    //if on an unsafe block, lose a life and return knight to start
    for (int i = 0; i <= unsafe_ctr; i++)
    {
        if(((sprite_y(knight)+sprite_height(knight)+1)==unsafe_blocks[i].y+1) && (sprite_x(knight)< unsafe_blocks[i].x+BLOCK_WIDTH) 
        &&(sprite_x(knight)+ KNIGHT_WIDTH -1 > unsafe_blocks[i].x)){
            lives -= 1;
            return_knight();
        }
    }
}
//compare block to last stepped on block, if different then score 
void compare_array(Sprite block){
    if((stepped_on[0].x == block.x) && (stepped_on[0].y == block.y)){
        score+=0;
        stepped_on[0].x = block.x;
        stepped_on[0].y = block.y;
    }else{
        score+=1;
        stepped_on[0].x = block.x;
        stepped_on[0].y = block.y;
    }
}

//checking for collision with a safe block
void check_safe_block(){
    //if on a safe block, allocate to variable
    for (int i = 0; i <= safe_ctr; i++)
    {
        int knight_y =sprite_y(knight)+sprite_height(knight)+1;
        int knight_x_right= sprite_x(knight)+ KNIGHT_WIDTH-1;

        if((knight_y == safe_blocks[i].y) && 
        (sprite_x(knight) < safe_blocks[i].x+BLOCK_WIDTH) &&
        (knight_x_right > safe_blocks[i].x)){
            compare_array(safe_blocks[i]);
        }
    }

}



void check_heart(){
    bool collision = true;

    int left_knight = round(sprite_x(knight));
    int left_heart = round(sprite_x(heart));
    int top_knight = round(sprite_y(knight));
    int top_heart = round(sprite_y(heart));
    int right_knight = left_knight + sprite_width(knight) - 1;
    int right_heart= left_heart + sprite_width(heart) - 1;
    int bottom_knight = top_knight + sprite_height(knight) - 1;
    int bottom_heart = top_heart + sprite_height(heart) - 1;

    if (left_knight > right_heart)
        collision= false;
    if (left_heart > right_knight)
        collision= false;
    if (top_knight > bottom_heart)
        collision= false;
    if (top_heart > bottom_knight)
        collision= false;

    if (collision == true){
        lives+= 2;
        heart->is_visible = false;
        return_knight();

    }
}

void setup(void)
{
    //start time
    StartTime = get_current_time();
    create_knight();
    create_blocks();
    create_treasure();
    update_draw();
}
void finish(){
    //calculate time
    int end = get_current_time();
    int time_spent = end -StartTime;
    int seconds = time_spent % 60;
    int minutes = time_spent / 60;

    clear_screen();
    draw_string(screen_width()/2 -10, screen_height()/2, "Game Over!");

    draw_string(screen_width()/2 -10, screen_height()/2 +1, "Score:");
    draw_int(screen_width()/2, screen_height()/2 + 1, score);

    draw_string(screen_width()/2-10, screen_height()/2 +2, "Time:");
    draw_int(screen_width()/2-4, screen_height()/2 +2, minutes );
    draw_string(screen_width()/2-1, screen_height()/2 +2, ":");
    draw_int(screen_width()/2, screen_height()/2 +2, seconds );

    draw_string(screen_width()/2-10, screen_height()/2 +3,"Press q to exit or r to Restart" );
    show_screen();

    //int Key = get_char();

}

//play one turn of game
void process()
{
    //run timer and get player input
    timer();
    int Key = get_char();

    //check blocks
    check_unsafe_block();
    check_safe_block();

    //gravity 
    gravity_acting = gravity();   
    if (gravity_acting == true){
        fall();
        sprite_set_image(knight, fall_knight_image);
    }else{ 
        move_knight(Key);
    }
    //treasure control
    toggle_heart(Key);
    if(heart_move == true){
        move_heart();
    }else{
        sprite_set_image(heart, broken_heart_image);
    }
    if(heart->is_visible == true){
        check_heart();
    }

    //check lives left

    update_draw();
    
}

//main entry point
int main(void)
{
    setup_screen();
    setup();
    while (!game_over)
    {
        process();
        timer_pause(DELAY);
        if (lives == 0){
            finish();
        }
    }
    wait_char();
    cleanup_screen();
    return 0;
}