# Conway's Game Of Life
Arduino code to run Conway's Game of Life on a NeoPixel Matrix

## Description
This project allows you to run a simulation of Conway's Game of Life on an Arduino connected to a NeoPixel Matrix.
Tested on Arduino Nano with an 8x8 matrix.

## Requirements
### Hardware
- Arduino
- NeoPixel Matrix
- Power supply to power the NeoPixels as desired
- Misc. hardware components to set everything up (Breadboard, jumper wires, buttons, USB cable)

### Software
- Arduino IDE
- Adafruit NeoMatrix library (installs Adafruit NeoPixel library too)

## Design Choices
- The canvas is considered endless, i.e, the matrix loops over from left-right edges and top-bottom edges
- The starting states are decided randomly
- The simulation runs until all cells are dead or until a fixed number of iterations have reached (to avoid getting stuck in an infinitely stable loop)
- If the simulation ends because all cells are dead, the matrix flashes red
- If the simulation ends because it has reached the max number of iterations, the matrix flashes green
- The new start state is again decided randomly

## Further planned enhancements
- Method to show intermediate states
  - Show an intermediate state where dying cells are red and new cells are green
  - Fade the new/dying cells
- Add buttons/knob to control brightness
- Add buttons/knob to control speed of next generation
- Add button to disable/enable intermediate states
