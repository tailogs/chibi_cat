# chibi_cat

**chibi_cat** - a simple cat widget that demonstrates cat animations.

## Description

`chibi_cat` is a lightweight and fun widget featuring animated cat graphics. The widget runs in a transparent window and displays various cat animations. It can be minimized to the system tray and interacts with an external process.

## Features

- Display various cat animations.
- Hide the application icon from the taskbar.
- Place an icon in the system tray with context menu options for managing the application.

Here’s the revised README text in English with improved structure and flow:

---

## Installation

Video Guide: [https://youtu.be/H08t6gD1Y1E?si=etF1ef9ZvxkGlrkL](https://youtu.be/H08t6gD1Y1E?si=etF1ef9ZvxkGlrkL)

All necessary `.dll`, `include`, and `lib` files are included in the repository. However, if you’d like to set everything up from scratch or encounter architecture conflicts, you can download the following packages:

- [SDL2-devel-2.30.8-mingw.zip](https://github.com/libsdl-org/SDL/releases/tag/releaseы=s)
- [SDL2_image-devel-2.8.2-mingw.zip](https://github.com/libsdl-org/SDL_image/releases)

Navigate to the `x86_64-w64-mingw32` directory for 64-bit systems (or the appropriate directory for other architectures), then copy the `lib` and `include` folders from both packages into the `src` folder of this project. Place the `.dll` files in the root of the project folder. Once this setup is complete, simply run:

```sh
make
```

### Steps to Set Up and Run

1. **Clone the repository:**

   ```sh
   git clone https://github.com/your_repository/chibi_cat.git
   ```

2. **Navigate to the project directory:**

   ```sh
   cd chibi_cat
   ```

3. **Build the project using the `Makefile`:**

   ```sh
   make
   ```

4. **Run the application:**

   ```sh
   chibi_cat.exe
   ```

--- 

This version should work well for the README.md file.

## Directory Structure

- `bin/` - Contains the compiled executable files.
- `sprite/` - Contains sprite images for cat animations.
- `src/` - Contains source code and headers.
- `tree_menu_utils/` - Contains utility files for the tree menu functionality.
- `other/` - Contains additional resources like images and PSD files.
- `icon.ico` - Application icon.
- `Makefile` - Build configuration file.
- `chibi_cat.exe` - Compiled executable for Windows.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contact

For more information, visit the [ITCH](https://tailogs.itch.io/chibi-cat) page or contact the project maintainers through the repository issues page.

## Screenshots

![image](https://github.com/user-attachments/assets/281bc502-5b8a-4fb8-96af-ebf614c0db6e)
