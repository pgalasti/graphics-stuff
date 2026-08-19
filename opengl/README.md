# OpenGL

My iterative improvements while I mostly read through https://learnopengl.com/

## Stuff you need to build

| Dependency |
|---|
| C++23 compiler |
| GLEW | 
| GLFW 3 |
| OpenGL (obviously)|
| glm, stb (I made as submodules)|

## Installing the libraries

### Debian / Ubuntu

```sh
sudo apt update
sudo apt install build-essential libglew-dev libglfw3-dev
```

### macOS

```sh
xcode-select --install
brew install glew glfw
```

### Linux display note

If you're not using Wayland, you can probably comment out ```glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);``` 
in each program. I'm not sure how these run on an X11 Linux platform.

## Submodules

Be sure to fetch the submodules:
```sh
git submodule update --init --recursive
```
## Building and running

Each program generally builds on the last program. Nothing fancy, just invoke make. 

Example:
```sh
cd 9.1_light-ambient
make
./light
```

I setup a macro to time operations in some programs. It can be set with the following:
```sh
make profile        # clean rebuild with PROFILER_ENABLE_INLINE_MACRO defined
```
