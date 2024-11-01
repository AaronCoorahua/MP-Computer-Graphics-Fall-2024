# MP-Computer-Graphics-Fall-2024

# Guardians of Gondor - MP: Over Hill and Under Hill

## Team Members
- **Aaron Inti (Aaron Coorahua)**
- **Byrne Valororn (Seth Motta)**
- **Ross (Ross Hodson)**

## Assignment
**MP - Over Hill and Under Hill**

## Project Description
This program renders a 3D scene with our three Heroes, each of whom can be moved around and viewed through an Arcball Camera relative to them. The program also implements:
- A first-person perspective for each Hero
- A Free Camera
- Custom Vertex and Fragment Shaders
- Illumination using a directional light, point light, and spotlight
- A textured skybox

## Running, Compiling, and Controls
The program should be compiled and run through CLion with the working directory set to `..`.

### Running the Program
1. Upon running, enter the name of the animation text file (`animation.txt`).

### Key Controls
- **WASD** - Move the selected Hero
- **Z, X, C** - Switch between Heroes
- **1** - Toggle first-person camera for the selected Hero
- **2** - Activate Free Camera (switch back by selecting a Hero)
- **3** - Activate animation/video mode
- **Q / Escape** - Close the program
- **Arrow Keys (Left/Right)** - When in Free-Cam mode, you can toggle between the first-person views of the Heroes using the left and right arrow keys after enabling the first-person view.

## Known Bugs
- **Crash on Minimizing**: The program will crash when attempting to minimize the window due to an issue with the `include/glm/ext/matrix_clip_space.inl` library file.

## Responsibility Distribution
- **Aaron** - Completed the entirety of Section A, Section B, and Extra Credit.
- **Seth** - Completed Sections C and D.
- **Ross** - Contributed to part of Section E.

## Time Spent
- This assignment took a total of **2-3 days** of work each.

## Feedback on Labs
- **Helpfulness**: The labs were very helpful for this assignment, rated 8/10.
- **Fun**: This assignment was engaging and encouraged consulting lecture slides and official documentation, rated 8/10.
