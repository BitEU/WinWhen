# WinWhen - ASCII Clock for Windows

A beautiful ASCII art digital clock that displays the current time and date in your Windows console. The clock features large, stylized numbers and automatically centers itself in the console window.

## Features

- **Large ASCII Art Display**: Time is shown using beautiful ASCII art characters
- **Real-time Updates**: Clock updates every second
- **Auto-centering**: Automatically centers the display in your console window
- **Date Display**: Shows the current date above the time
- **Dynamic Console Sizing**: Automatically resizes the console window to fit the clock
- **HH:MM:SS Format**: Displays hours, minutes, and seconds with colons

## Screenshots

The clock displays time in a format like this:
```
                                                                                     Friday, July 4, 2025






               000000000        1111111                   000000000       333333333333333                1111111         999999999
             00:::::::::00     1::::::1                 00:::::::::00    3:::::::::::::::33             1::::::1       99:::::::::99
           00:::::::::::::00  1:::::::1               00:::::::::::::00  3::::::33333::::::3           1:::::::1     99:::::::::::::99
          0:::::::000:::::::0 111:::::1              0:::::::000:::::::0 3333333     3:::::3           111:::::1    9::::::99999::::::9
          0::::::0   0::::::0    1::::1              0::::::0   0::::::0             3:::::3              1::::1    9:::::9     9:::::9
          0:::::0     0:::::0    1::::1      ::::::  0:::::0     0:::::0             3:::::3   ::::::     1::::1    9:::::9     9:::::9
          0:::::0     0:::::0    1::::1      ::::::  0:::::0     0:::::0     33333333:::::3    ::::::     1::::1     9:::::99999::::::9
          0:::::0 000 0:::::0    1::::l      ::::::  0:::::0 000 0:::::0     3:::::::::::3     ::::::     1::::l      99::::::::::::::9
          0:::::0 000 0:::::0    1::::l              0:::::0 000 0:::::0     33333333:::::3               1::::l        99999::::::::9
          0:::::0     0:::::0    1::::l              0:::::0     0:::::0             3:::::3              1::::l             9::::::9
          0:::::0     0:::::0    1::::l              0:::::0     0:::::0             3:::::3              1::::l            9::::::9
          0::::::0   0::::::0    1::::l      ::::::  0::::::0   0::::::0             3:::::3   ::::::     1::::l           9::::::9
          0:::::::000:::::::0 111::::::111   ::::::  0:::::::000:::::::0 3333333     3:::::3   ::::::  111::::::111       9::::::9
           00:::::::::::::00  1::::::::::1   ::::::   00:::::::::::::00  3::::::33333::::::3   ::::::  1::::::::::1      9::::::9
             00:::::::::00    1::::::::::1              00:::::::::00    3:::::::::::::::33            1::::::::::1     9::::::9
               000000000      111111111111                000000000       333333333333333              111111111111    99999999
```

## Project Structure

- `ascii_clock.c` - Main clock program source code
- `ascii_clock.exe` - Compiled clock executable
- `generate_chars.c` - Utility to generate ASCII character data from text files
- `generate_chars.exe` - Compiled generator utility
- `ascii_chars.h` - Generated header file containing ASCII art data
- `zero.txt` through `nine.txt` - ASCII art templates for digits 0-9
- `colon.txt` - ASCII art template for the colon character

## How to Compile Your Shit

### Prerequisites
- Windows with Visual Studio Build Tools or Visual Studio installed
- Microsoft C/C++ Compiler (MSVC)

### Compilation Steps

1. **Open VS Developer CMD prompt**
   - Search for "Developer Command Prompt" in your Start menu
   - Or open a regular command prompt and run `vcvars64.bat` from your Visual Studio installation

2. **Navigate to project directory**
   ```cmd
   cd "c:\Users\Steven\Github\WinWhen"
   ```

3. **Compile the character generator**
   ```cmd
   cl generate_chars.c /Fe:generate_chars.exe
   ```

4. **Run the character generator** (this creates `ascii_chars.h`)
   ```cmd
   generate_chars.exe
   ```

5. **Compile the main clock program**
   ```cmd
   cl ascii_clock.c /Fe:ascii_clock.exe
   ```

6. **Run the clock**
   ```cmd
   ascii_clock.exe
   ```

### Alternative One-liner Compilation
If you want to compile everything in one go:
```cmd
cl generate_chars.c /Fe:generate_chars.exe && generate_chars.exe && cl ascii_clock.c /Fe:ascii_clock.exe
```

## Usage

Simply run the compiled executable:
```cmd
ascii_clock.exe
```

- The clock will automatically resize your console window to fit properly
- Press `Ctrl+C` to exit the program
- The display updates every second with the current time

## Customization

### Modifying ASCII Art
To customize the appearance of the digits:

1. Edit the `.txt` files (`zero.txt`, `one.txt`, etc.) with your preferred ASCII art
2. Recompile using the steps above (you must run `generate_chars.exe` after editing)

### Technical Details
- Maximum character dimensions: 30 lines × 20 columns
- Supports digits 0-9 and colon character
- Uses Windows API for console manipulation
- Automatically calculates centering based on console width

## Requirements

- Windows operating system
- Console/Command Prompt
- Visual Studio Build Tools or Visual Studio (for compilation)

## License

This project is licensed under the terms found in the LICENSE file.

## Troubleshooting

**Q: The clock doesn't display properly**
- Make sure your console window is large enough
- Try running in full screen mode
- Ensure you have proper Windows console font settings

**Q: Compilation errors**
- Make sure you're using the Visual Studio Developer Command Prompt
- Verify all `.txt` files are present in the directory
- Check that `generate_chars.exe` ran successfully and created `ascii_chars.h`

**Q: Clock appears off-center**
- The program automatically calculates centering, but very narrow console windows may cause issues
- Try manually resizing your console window wider