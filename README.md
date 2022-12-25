# NESEmu

<p align="center">
  <img width="900" height="350" alt="cover image showing prehistorik man" src="https://raw.githubusercontent.com/BlueBlazin/nesemu/master/screenshots/smb.png">
</p>

An NES emulator written in C++. Currently supports NROM and UxROM cartridges.

## Background

I mainly wrote this to learn C++. I started working on it in the July of 2022 and it took around 3 months to finish. I was completely new to C++ at the time so you find some quirky or weird usage of the language in this codebase.

## Requirements

In order to run the emulator you first need:

- SFML
- SDL

Why both SFML and SDL? Frankly, because I didn't know any better. I started with SFML and upon reaching the audio implementation realized it wasn't going to cut it. So I used SDL for audio.

Next you'll also need Bazel. You could, if you want, write a Makefile for this. But if not, you need Bazel.

Finally, you'll need to modify the `BUILD.bazel`s in `third_party/` with the locations where you installed SFML and SDL. You'll also need to modify `WORKSPACE.bazel` for the same reason.

## Running The Emulator

First build the binary.

```sh
bazel build src:main --cxxopt='-std=c++20' --copt=-O3
```

It is imperative that the binary is built using `O3` optimization as this is required to smoothly run the emulator. But the codebase is small so this shouldn't take too long.

Then, run your game, for example:

```sh
./bazel-bin/src/main Contra.nes
```

That's it! Shoutout and big thanks to the 'NES Development Server' discord community!
