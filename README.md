# Lidar Doom Engine

## Description
The **Lidar Doom Engine** is a custom 3D rendering engine built using OpenGL and GLUT. The engine features a minimalistic design, focusing on basic sector-based rendering, wall drawing, and mouse-controlled player navigation. It includes elements such as jump physics, dynamic wall interaction, and a crosshair system.

## Features
- **Sector-Based Rendering:** Walls and surfaces are grouped into sectors for efficient rendering and logical grouping.
- **Player Movement:** Navigate with keyboard keys and look around with the mouse. Supports jumping and strafing.
- **Mouse Interaction:**
  - Dynamic crosshair changes color when clicking.
  - Wall selection and color modification via mouse clicks.
- **Physics Simulation:** Jumping and gravity mechanics with collision detection.
- **Dynamic Rendering:** Frame-based rendering ensures smooth updates.

## Requirements
- **Libraries**:
  - OpenGL
  - GLUT (FreeGLUT recommended)
- **Operating System**: Windows (utilizes `windows.h` for system metrics)
- **Compiler**: Any C/C++ compiler compatible with OpenGL and GLUT.

## Controls
### Keyboard
- `W`: Move forward
- `S`: Move backward
- `,`: Turn left
- `.`: Turn right
- `A`: Strafe left
- `D`: Strafe right
- `Space`: Jump

### Mouse
- **Movement:** Look around using the mouse.
- **Left Click:** Change the color of clicked walls and the crosshair.
- **Exit/Enter Window:** Hides the mouse cursor when inside the window.

## Setup and Installation
1. **Install Dependencies**: Ensure OpenGL and GLUT are installed on your system.
2. **Compile the Code**:
   Use the following command with a GCC-compatible compiler:
   ```bash
   gcc main.c -o LidarDoomEngine -lglut -lGL -lm
   ```
   On Windows, include `-lopengl32` and `-lfreeglut`.
3. **Run the Executable**:
   ```bash
   ./LidarDoomEngine
   ```

## Code Structure
### Key Components
- **`player` Struct**: Stores player position, angle, and jump states.
- **`walls` Struct**: Represents wall coordinates and colors.
- **`sectors` Struct**: Handles groups of walls with height and surface attributes.
- **Rendering Functions:**
  - `draw3D()`: Draws sectors and walls.
  - `drawWall()`: Handles wall drawing with depth sorting.
  - `clearBackground()`: Resets the screen each frame.
- **Physics Functions:**
  - `movePlayer()`: Processes player movement and jumping.
  - `clipBehindPlayer()`: Ensures objects are clipped if behind the player.

### Mouse Interaction
- `mouseClick`: Handles wall selection and dynamic color changes.
- `mouseMove`: Updates player angle based on mouse movement.
- `mouseEnterLeave`: Detects when the mouse enters or exits the window.

### Display Loop
- `display()`: The main rendering loop, called by GLUT's callback system. Includes background clearing, player updates, and 3D rendering.

## Visuals
The engine features a retro-style rendering with:
- Basic geometric shapes.
- Vibrant colors for walls and sectors.
- Dynamic crosshair for targeting.

## Future Enhancements
- Add texture mapping for walls and surfaces.
- Implement collision detection for realistic movement.
- Support additional input devices and controls.
- Optimize rendering pipeline for improved performance.

## Author
James Aliev

