#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include "ascii_chars.h"

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

// Function to display the time
void display_time() {
    time_t rawtime;
    struct tm* timeinfo;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    // Format time as HH:MM:SS
    char time_str[9];
    sprintf(time_str, "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    
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
    
    // Get console dimensions
    int console_width, console_height;
    get_console_size(&console_width, &console_height);
    
    // Calculate total width of the clock display
    int total_clock_width = (max_width * 8) + 7 + 4; // 8 chars, 7 normal spaces, 4 extra spaces for colons
    int clock_padding = (console_width - total_clock_width) / 2;
    if (clock_padding < 0) clock_padding = 0;
    
    // Clear screen
    system("cls");
    
    // Display date at the top
    char date_str[MAX_DATE_LENGTH];
    get_date_string(date_str, sizeof(date_str));
    printf("\n");
    print_centered(date_str, console_width);
    printf("\n\n");
    
    // Display each line of the ASCII art (centered)
    for (int line = 0; line < max_height; line++) {
        // Print padding to center the clock
        for (int i = 0; i < clock_padding; i++) {
            printf(" ");
        }
        
        for (int char_pos = 0; char_pos < TIME_CHARS; char_pos++) {
            int char_idx = char_indices[char_pos];
            
            // Print the character line
            if (line < ascii_chars[char_idx].height) {
                printf("%s", ascii_chars[char_idx].lines[line]);
            } else {
                // Print spaces if this character doesn't have this many lines
                for (int i = 0; i < max_width; i++) {
                    printf(" ");
                }
            }
            
            // Add spacing between characters
            if (char_pos < TIME_CHARS - 1) {
                if (char_pos == 1 || char_pos == 4) {
                    // Two spaces before colon
                    printf("  ");
                } else {
                    // Single space between other characters
                    printf(" ");
                }
            }
        }
        printf("\n");
    }
}

int main() {
    // Load ASCII characters
    if (!load_ascii_chars()) {
        printf("Error: Could not load embedded ASCII character data.\n");
        return 1;
    }
    
    // Calculate required console size
    int max_height, max_width;
    get_max_dimensions(&max_height, &max_width);
    
    // Calculate total width needed: 8 characters + 7 spaces (1 between most, 2 before colons)
    // Actually: digit + space + digit + 2spaces + colon + 2spaces + digit + space + digit + 2spaces + colon + 2spaces + digit + space + digit
    int total_width = (max_width * 8) + 7 + 4; // 8 chars, 7 normal spaces, 4 extra spaces for colons
    
    // Make console wide enough to center the clock with some margin
    int console_width = total_width + 20; // Add 20 chars margin
    
    // Resize console window
    resize_console(console_width, max_height);
    
    printf("ASCII Clock - Press Ctrl+C to exit\n\n");
    
    // Main loop - update every second
    while (1) {
        display_time();
        Sleep(1000); // Sleep for 1 second
    }
    
    return 0;
}
