# cg-minecart-yasmined2467903-katerinag2470179

A Minecraft inspired university project.

## Camera Controls
- `WASD` : Move forward, backward, left, right
- `Left Shift` : Move down
- `Space` : Move up
- `Right Mouse Drag` : Camera angle

## Other Controls
- `P` : Cycle post-processing effects (None, Blur, Sobel, Chroma Key, Bloom)
- `B` : Toggle Bloom
- `N` : Toggle day/night
- `F` : Toggle first-person view
- `ESC` : Exit window
- `Left Mouse Click` : Toggle torches on and off (have to click on tip of torch)

## Compile and run the project on Linux
From inside the `minecart` folder, do
1. (If there is a pre-existing `build` folder) `rm -rf folder`
2. `cmake -S . -B build`
3. `cmake --build build -j$(nproc)`
4. `./build/minecart`

## Vcpkg install
We installed assimp through vcpkg, make sure to do so before running the program else it will not run
when assimp is installed, run `integrate install` to make sure that it will be used in the project as well