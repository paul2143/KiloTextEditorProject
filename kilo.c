/*** includes ***/
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

/*** defines ***/

#define CRTL_KEY(k) ((k) & 0x1f)


/*** data ***/

struct termios original_terminal;

/**
 * Clears the screen
 */
void editorWipeScreen() {

    // Clear the entire screen
    write(STDOUT_FILENO, "\x1b[2J", 4);

    // Move the cursor to the top left of the screen
    write(STDOUT_FILENO, "\x1b[H", 3);
}

/*** terminal ***/

/**
 * Provides an error handling function and kills the program in the event of an error
 */
void die(const char *s) {
    editorWipeScreen();
    perror(s);
    exit(1);
}

/**
 * Returns the console to its original state
 */
void disableRawMode(){
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_terminal)) die("tcsetattr");
}


void enableRawMode(){

    // get the attributes associated with the terminal and assign them to original
    if (tcgetattr(STDIN_FILENO, &original_terminal) == -1) die("tcgetattr");
    
    // ensure that disableRawMode is called at exit
    atexit(disableRawMode);
    
    // set raw = t0 the original
    struct termios raw = original_terminal;
    
    // Set the terminal into raw mode
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_iflag &= ~(ICRNL | IXON | ISTRIP | INPCK | BRKINT);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    
    // Allow read to return with 0 bytes
    raw.c_cc[VMIN] = 0;

    // Maximum time before a read returns in tenths of seconds
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
    
}

/**
 * Read the keypress
 * 
 * 
 */
char editorReadKey() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if(nread == -1 && errno != EAGAIN) die("read");
    }
    return c;
}

/*** output ***/

void editorDrawRows() {
    for( int x = 0; x <24; x++){
        write(STDOUT_FILENO, "~\r\n", 3);
    }
}

void editorRefreshScreen() {
    
}


/*** input ***/


/**
 * Logic responsible for processing a read keypress
 * 
 * 
 */
void editorProcessKeypress() {
    char c = editorReadKey();
    switch (c)
    {
        case CRTL_KEY('q'):
            editorWipeScreen();
            exit(0);
            break;
    default:
        break;
    }
}

/*** init ***/

/**
 * 
 * 
 * 
 */
int main() {
    enableRawMode();
    while(1) {
        editorProcessKeypress();
    }
    return 0;
}