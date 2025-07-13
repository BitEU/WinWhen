#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include "ascii_chars.h"

// For cursor positioning and selective redraw
static char previous_time_str[9] = {0};
static int first_run = 1;

// Double buffering screen handles
static HANDLE hScreen1, hScreen2;
static HANDLE hCurrentScreen;
static int current_buffer = 0;

#define MAX_LINES 30
#define MAX_COLS 20
#define NUM_DIGITS 10
#define TIME_CHARS 8  // HH:MM:SS format
#define MAX_DATE_LENGTH 50

// Structure to hold ASCII art for a character
typedef struct {
    char lines[MAX_LINES][MAX_COLS + 1];
    int height;
    int width;
} ASCIIChar;

// Global array to store all ASCII characters (0-9 and colon)
ASCIIChar ascii_chars[NUM_DIGITS + 1];  // 0-9 plus colon

// Function to load embedded ASCII characters
int load_ascii_chars() {
    // Create array for embedded character data
    EmbeddedASCIIChar embedded_chars[NUM_DIGITS + 1];
    
    // Initialize the embedded character data
    init_embedded_ascii_chars(embedded_chars);
    
    // Copy from embedded data to our global structure
    for (int i = 0; i < NUM_DIGITS + 1; i++) {
        const EmbeddedASCIIChar* src = &embedded_chars[i];
        ASCIIChar* dest = &ascii_chars[i];
        
        dest->height = src->height;
        dest->width = src->width;
        
        // Copy lines from embedded data to our structure
        for (int line = 0; line < src->height && line < MAX_LINES; line++) {
            if (src->lines[line] != NULL) {
                strncpy(dest->lines[line], src->lines[line], MAX_COLS);
                dest->lines[line][MAX_COLS] = '\0';
            } else {
                dest->lines[line][0] = '\0';
            }
        }
        
        // Initialize remaining lines to empty strings
        for (int line = src->height; line < MAX_LINES; line++) {
            dest->lines[line][0] = '\0';
        }
    }
    return 1;
}

// Function to get the maximum dimensions
void get_max_dimensions(int* max_height, int* max_width) {
    *max_height = 0;
    *max_width = 0;
    
    for (int i = 0; i < NUM_DIGITS + 1; i++) {
        if (ascii_chars[i].height > *max_height) {
            *max_height = ascii_chars[i].height;
        }
        if (ascii_chars[i].width > *max_width) {
            *max_width = ascii_chars[i].width;
        }
    }
}

// Function to resize console window
void resize_console(int width, int height) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD newSize = {width, height + 10}; // Add extra lines for date and spacing
    
    // Set the screen buffer size first
    SetConsoleScreenBufferSize(hConsole, newSize);
    
    // Set the window size
    SMALL_RECT windowSize = {0, 0, width - 1, height + 9};
    SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
}

// Function to get console dimensions
void get_console_size(int* width, int* height) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        *width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        *height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    } else {
        *width = 80;  // Default fallback
        *height = 25;
    }
}

// Function to get formatted date string
void get_date_string(char* date_str, size_t max_len) {
    time_t rawtime;
    struct tm* timeinfo;
    const char* weekdays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    const char* months[] = {"January", "February", "March", "April", "May", "June", 
                           "July", "August", "September", "October", "November", "December"};
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    snprintf(date_str, max_len, "%s, %s %d, %d", 
             weekdays[timeinfo->tm_wday],
             months[timeinfo->tm_mon],
             timeinfo->tm_mday,
             timeinfo->tm_year + 1900);
}

// Function to print centered text
void print_centered(const char* text, int console_width) {
    int text_len = strlen(text);
    int padding = (console_width - text_len) / 2;
    
    if (padding > 0) {
        for (int i = 0; i < padding; i++) {
            printf(" ");
        }
    }
    printf("%s\n", text);
}

// Function to initialize double buffering
int init_double_buffer() {
    // Create two screen buffers
    hScreen1 = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        CONSOLE_TEXTMODE_BUFFER,
        NULL
    );
    
    hScreen2 = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        CONSOLE_TEXTMODE_BUFFER,
        NULL
    );
    
    if (hScreen1 == INVALID_HANDLE_VALUE || hScreen2 == INVALID_HANDLE_VALUE) {
        return 0; // Failed to create buffers
    }
    
    // Start with screen1 as active
    hCurrentScreen = hScreen1;
    SetConsoleActiveScreenBuffer(hCurrentScreen);
    
    return 1; // Success
}

// Function to swap screen buffers
void swap_buffers() {
    current_buffer = 1 - current_buffer;
    hCurrentScreen = (current_buffer == 0) ? hScreen1 : hScreen2;
    SetConsoleActiveScreenBuffer(hCurrentScreen);
}

// Function to clear entire screen buffer without flashing
void clear_screen_buffer(HANDLE hBuffer) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD coord = {0, 0};
    DWORD written;
    
    if (GetConsoleScreenBufferInfo(hBuffer, &csbi)) {
        DWORD bufferSize = csbi.dwSize.X * csbi.dwSize.Y;
        FillConsoleOutputCharacter(hBuffer, ' ', bufferSize, coord, &written);
        FillConsoleOutputAttribute(hBuffer, csbi.wAttributes, bufferSize, coord, &written);
        SetConsoleCursorPosition(hBuffer, coord);
    }
}

// Function to clear a specific area of the screen buffer
void clear_area(HANDLE hBuffer, int start_line, int num_lines, int console_width) {
    COORD coord;
    DWORD written;
    
    for (int i = 0; i < num_lines; i++) {
        coord.X = 0;
        coord.Y = start_line + i;
        SetConsoleCursorPosition(hBuffer, coord);
        FillConsoleOutputCharacter(hBuffer, ' ', console_width, coord, &written);
    }
}

// Function to position cursor on specific buffer
void set_cursor_position(HANDLE hBuffer, int x, int y) {
    COORD coord = {x, y};
    SetConsoleCursorPosition(hBuffer, coord);
}

// Function to hide/show cursor on specific buffer
void set_cursor_visibility(HANDLE hBuffer, int visible) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hBuffer, &cursorInfo);
    cursorInfo.bVisible = visible;
    SetConsoleCursorInfo(hBuffer, &cursorInfo);
}

// Function to write text to specific buffer at current position
void write_to_buffer(HANDLE hBuffer, const char* text) {
    DWORD written;
    WriteConsole(hBuffer, text, strlen(text), &written, NULL);
}

// Function to write centered text to buffer
void write_centered_to_buffer(HANDLE hBuffer, const char* text, int console_width) {
    int text_len = strlen(text);
    int padding = (console_width - text_len) / 2;
    
    if (padding > 0) {
        for (int i = 0; i < padding; i++) {
            write_to_buffer(hBuffer, " ");
        }
    }
    write_to_buffer(hBuffer, text);
    write_to_buffer(hBuffer, "\n");
}

// Function to display the time
void display_time() {
    time_t rawtime;
    struct tm* timeinfo;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    // Format time as HH:MM:SS
    char time_str[9];
    sprintf(time_str, "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    
    // Check if time has changed (skip redraw if same)
    if (!first_run && strcmp(time_str, previous_time_str) == 0) {
        return;
    }
    
    // Get the inactive buffer for drawing
    HANDLE hBackBuffer = (current_buffer == 0) ? hScreen2 : hScreen1;
    
    // Clear the back buffer completely
    clear_screen_buffer(hBackBuffer);
    
    // Hide cursor on back buffer
    set_cursor_visibility(hBackBuffer, 0);
    
    // Get character indices for display
    int char_indices[8];
    char_indices[0] = time_str[0] - '0';  // First hour digit
    char_indices[1] = time_str[1] - '0';  // Second hour digit
    char_indices[2] = NUM_DIGITS;         // Colon (index 10)
    char_indices[3] = time_str[3] - '0';  // First minute digit
    char_indices[4] = time_str[4] - '0';  // Second minute digit
    char_indices[5] = NUM_DIGITS;         // Colon (index 10)
    char_indices[6] = time_str[6] - '0';  // First second digit
    char_indices[7] = time_str[7] - '0';  // Second second digit
    
    // Get maximum height for proper alignment
    int max_height, max_width;
    get_max_dimensions(&max_height, &max_width);
    
    // Use consistent width for all characters to prevent shifting
    int uniform_width = max_width;
    
    // Get console dimensions
    int console_width, console_height;
    get_console_size(&console_width, &console_height);
    
    // Calculate total width of the clock display using uniform width
    int total_clock_width = (uniform_width * 8) + 7 + 4; // 8 chars, 7 normal spaces, 4 extra spaces for colons
    int clock_padding = (console_width - total_clock_width) / 2;
    if (clock_padding < 0) clock_padding = 0;
    
    // Display date at the top
    char date_str[MAX_DATE_LENGTH];
    get_date_string(date_str, sizeof(date_str));
    
    // Position cursor and write date
    set_cursor_position(hBackBuffer, 0, 1);
    write_centered_to_buffer(hBackBuffer, date_str, console_width);
    
    // Calculate where the clock starts (line 4 after title and spacing)
    int clock_start_line = 4;
    
    // Display each line of the ASCII art (centered)
    for (int line = 0; line < max_height; line++) {
        // Position cursor at the start of this line
        set_cursor_position(hBackBuffer, 0, clock_start_line + line);
        
        // Create line content in memory first
        char line_content[512] = {0}; // Buffer for the entire line
        int pos = 0;
        
        // Add padding to center the clock
        for (int i = 0; i < clock_padding; i++) {
            line_content[pos++] = ' ';
        }
        
        for (int char_pos = 0; char_pos < TIME_CHARS; char_pos++) {
            int char_idx = char_indices[char_pos];
            
            // Add the character line with uniform width
            if (line < ascii_chars[char_idx].height) {
                // Add the character content
                const char* char_line = ascii_chars[char_idx].lines[line];
                int char_actual_width = strlen(char_line);
                strcpy(line_content + pos, char_line);
                pos += char_actual_width;
                
                // Pad to uniform width
                for (int i = char_actual_width; i < uniform_width; i++) {
                    line_content[pos++] = ' ';
                }
            } else {
                // Add spaces for uniform width if this character doesn't have this many lines
                for (int i = 0; i < uniform_width; i++) {
                    line_content[pos++] = ' ';
                }
            }
            
            // Add spacing between characters
            if (char_pos < TIME_CHARS - 1) {
                if (char_pos == 1 || char_pos == 4) {
                    // Two spaces before colon
                    line_content[pos++] = ' ';
                    line_content[pos++] = ' ';
                } else {
                    // Single space between other characters
                    line_content[pos++] = ' ';
                }
            }
        }
        
        line_content[pos++] = '\n';
        line_content[pos] = '\0';
        
        // Write the entire line to buffer at once
        write_to_buffer(hBackBuffer, line_content);
    }
    
    // Swap buffers to display the new frame instantly
    swap_buffers();
    
    // Update previous time and first run flag
    strcpy(previous_time_str, time_str);
    first_run = 0;
}

int main() {
    // Load ASCII characters
    if (!load_ascii_chars()) {
        printf("Error: Could not load embedded ASCII character data.\n");
        return 1;
    }
    
    // Initialize double buffering
    if (!init_double_buffer()) {
        printf("Error: Could not initialize double buffering.\n");
        return 1;
    }
    
    // Calculate required console size
    int max_height, max_width;
    get_max_dimensions(&max_height, &max_width);
    
    // Calculate total width needed using uniform character width
    int total_width = (max_width * 8) + 7 + 4; // 8 chars, 7 normal spaces, 4 extra spaces for colons
    
    // Make console wide enough to center the clock with some margin
    int console_width = total_width + 20; // Add 20 chars margin
    
    // Resize console window for both buffers
    resize_console(console_width, max_height);
    
    // Set screen buffer size for both buffers
    COORD newSize = {console_width, max_height + 10};
    SetConsoleScreenBufferSize(hScreen1, newSize);
    SetConsoleScreenBufferSize(hScreen2, newSize);
    
    // Clear the initial screen and show startup message
    clear_screen_buffer(hCurrentScreen);
    set_cursor_position(hCurrentScreen, 0, 0);
    write_to_buffer(hCurrentScreen, "ASCII Clock - Press Ctrl+C to exit\n\n");
    
    // Main loop - update every second
    while (1) {
        display_time();
        Sleep(1000); // Sleep for 1 second
    }
    
    return 0;
}
