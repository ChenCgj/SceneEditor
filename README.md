# SceneEditor
CG course final project

## build

I use the VSC to develop.

The compiler is Mingw's gcc version 11.2.0. The library are prepared for the compiler. If you use VS(MSVC) you need to find the corresponding library for MSVC.

I think the .vscode folder help you configure the environment. What you need is to add the path of the MINGW to the system path.
Check it by runing `mingw32-make` and `g++ -v` in your CLI.

You also can configure other IDE or editor according to the makefile.
