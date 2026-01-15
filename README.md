# SD2 9999 Editor

<img src="https://github.com/user-attachments/assets/96a8079a-2cb3-4984-98aa-e42d953373aa" alt="SD2 9999 Editor Screenshot" width="800"/>

A small open-source ImGui-based desktop tool for viewing and editing the `9999.dat` move data file used by *WWF SmackDown! 2: Know Your Role*.

This is primarily a learning project focused on:

* Binary file parsing and preservation
* Immediate-mode GUI design with Dear ImGui
* Clean, minimal C++ application structure

## Features

* View move sections inside 9999.dat
* Edit move ID and type (category) values
* Add and delete entries
* Preserve unknown trailing data at the end of the file

## Build Instructions

### Requirements

* C++20 compatible compiler
* OpenGL 3.3+
* GLFW
* GLAD
* Dear ImGui

### Building (Visual Studio – Windows)

1. Clone the repository:

   ```bash
   git clone https://github.com/CactusBack/sd2-9999-editor.git
   ```

2. Open the project in Visual Studio

3. Make sure the following are available:

   * GLFW linked
   * GLAD source included
   * Dear ImGui source included

4. Build and run

> Note: This project currently targets Windows + OpenGL. Other platforms may require adjustments.

## Usage

1. Place `9999.dat` in the same directory as the executable
2. Launch the editor
3. Select a section
4. Edit values as needed
5. Click **Save File** to write changes back

## License

MIT License

See `LICENSE.txt` file for details.

## Third-Party Assets

This project includes the **Inconsolata** font, licensed under the  **SIL Open Font License (OFL)**.

Font source: https://github.com/googlefonts/Inconsolata

## AI-Assisted Development Disclosure

This project was developed with assistance from **ChatGPT (OpenAI)**.

AI assistance was used for:

* Code cleanup and refactoring suggestions
* Dear ImGui usage guidance
* C++ best-practice discussions

All design decisions, testing, and final implementation choices were made by the project author.

## Disclaimer

This tool is not affiliated with Sony, WWE, THQ or Yuke’s.
It is intended for educational and personal modding use only.

## Contributing

Pull requests are welcome. If you have ideas for improvements or refactors, feel free to open an issue.

## Contact Email

smackdown2modded@gmail.com
