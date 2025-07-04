#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 30
#define MAX_COLS 20

int main() {
    const char* filenames[] = {
        "zero.txt", "one.txt", "two.txt", "three.txt", "four.txt",
        "five.txt", "six.txt", "seven.txt", "eight.txt", "nine.txt",
        "colon.txt"
    };
    
    const char* varnames[] = {
        "zero", "one", "two", "three", "four",
        "five", "six", "seven", "eight", "nine",
        "colon"
    };
    
    FILE* header = fopen("ascii_chars.h", "w");
    if (!header) {
        printf("Error: Could not create ascii_chars.h\n");
        return 1;
    }
    
    fprintf(header, "#ifndef ASCII_CHARS_H\n");
    fprintf(header, "#define ASCII_CHARS_H\n\n");
    fprintf(header, "// Auto-generated file containing ASCII character data\n\n");
    
    // Generate data for each character
    for (int i = 0; i < 11; i++) {
        FILE* file = fopen(filenames[i], "r");
        if (!file) {
            printf("Error: Could not open %s\n", filenames[i]);
            fclose(header);
            return 1;
        }
        
        fprintf(header, "// Data for %s\n", filenames[i]);
        fprintf(header, "static const char* %s_lines[] = {\n", varnames[i]);
        
        char buffer[256];
        int line_count = 0;
        int max_width = 0;
        
        while (fgets(buffer, sizeof(buffer), file) && line_count < MAX_LINES) {
            // Remove newline character
            int len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') {
                buffer[len - 1] = '\0';
                len--;
            }
            
            if (len > max_width) {
                max_width = len;
            }
            
            // Escape special characters and write the line
            fprintf(header, "    \"");
            for (int j = 0; j < len; j++) {
                if (buffer[j] == '\\') {
                    fprintf(header, "\\\\");
                } else if (buffer[j] == '"') {
                    fprintf(header, "\\\"");
                } else {
                    fprintf(header, "%c", buffer[j]);
                }
            }
            fprintf(header, "\",\n");
            line_count++;
        }
        
        fprintf(header, "    NULL\n};\n");
        fprintf(header, "static const int %s_height = %d;\n", varnames[i], line_count);
        fprintf(header, "static const int %s_width = %d;\n\n", varnames[i], max_width);
        
        fclose(file);
    }
    
    // Generate initialization function instead of static array
    fprintf(header, "// Combined character data structure\n");
    fprintf(header, "typedef struct {\n");
    fprintf(header, "    const char** lines;\n");
    fprintf(header, "    int height;\n");
    fprintf(header, "    int width;\n");
    fprintf(header, "} EmbeddedASCIIChar;\n\n");
    
    fprintf(header, "// Function to initialize the character data\n");
    fprintf(header, "static void init_embedded_ascii_chars(EmbeddedASCIIChar* chars) {\n");
    for (int i = 0; i < 11; i++) {
        fprintf(header, "    chars[%d].lines = %s_lines;\n", i, varnames[i]);
        fprintf(header, "    chars[%d].height = %s_height;\n", i, varnames[i]);
        fprintf(header, "    chars[%d].width = %s_width;\n", i, varnames[i]);
    }
    fprintf(header, "}\n\n");
    
    fprintf(header, "#endif // ASCII_CHARS_H\n");
    fclose(header);
    
    printf("Generated ascii_chars.h successfully!\n");
    return 0;
}
