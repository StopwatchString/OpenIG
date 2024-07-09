# OpenIG

## Setting up the development environment
- Clone this repo recursively to pull down git submodules in /extern
    - GLFW for window management
    - GLM for math
- Install a VulkanSDK (Lunarg is a good choice)
- Run the script gen_solution.bat at the base of the repo, which simply runs CMake generation and places the solution in the /build folder