/*** includes ***/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*** defines ***/

#define CTRL_KEY(k) ((k) & 0x1f) //bitwise & with 00011111- same as what CTRL does in terminal

/*** data ***/

struct termios orig_termios;

/*** terminal ***/

void die(const char *s) {
    perror(s);
    exit(1);
}

void disableRawmode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawmode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) //Getting terminal attributes
        die("tcgetattr");
    atexit(disableRawmode); //Diable Rawmode at exit

    struct termios raw = orig_termios; //cpy struct from original
    raw.c_lflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_lflag &= ~(OPOST);
    raw.c_lflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); //Bitwise AND the flag with ECHO and ICANON, etc flags as 1s
    raw.c_cc[VMIN] = 0; //min no. of bytes to read as input
    raw.c_cc[VTIME] = 1; //max time to wait before read returns

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) //Set the new struct
        die("tcsetattr");
}

char editorReadKey() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) die ("read");
    }
    return c;
}

/*** input ***/

void editorProcessKeypress() {
    char c = editorReadKey();

    switch (c) {
        case CTRL_KEY('q');
            exit(0);
            break;
    }
}

/*** init ***/

int main() {
    enableRawmode();

    //read chars 1 by 1, read returns 1 if there is a character, 'q' to quit
    while (1) {
        editorProcessKeypress();
    }

    return 0;
}