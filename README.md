# ADK (Ayax Development Kit)

### "Hey, good luck on your engine." - [Noel Berry](https://twitter.com/NoelFB) Aug 14, 2020 at a bouldering gym

<img src="https://i.imgur.com/4ze1jne.png" alt="ayax logo" width="200" height="200">*Greek mythological hero, Ajax the Great*

## Usage
Just fork the repo and get started. Everything this engine needs is included within this repository. I'm working on ADK using Visual Studio 2019. If you want to compile ADK, you should consider using this IDE. ADK only targets Windows x32. The VS project files are configured to compile with all the dependenices.

## Purpose
ADK is a few things. ADK is a C++ game engine providing tools for building modern 2D games. ADK is a collection of useful game systems such as pathfinding, inventory, and character controllers. ADK is the open source repository for several games that I'm working on. Essentially, I'm trying to provide all the resources and tools that one would want in order to create a 2D video game. Whether you take one of the example/demo games and expand upon it or decide to create a new game using the integrated editor, ADK provides the features you need.

<img src="https://i.imgur.com/HRosCqM.png" alt="ayax screenshot 1" width="854" height="480">

I started this little project because I wanted two things: I wanted my own custom C++ game engine to make games with and I wanted a monolithic repository where I can collect all of my little games and game-related programming projects. Instead of having a ton of small projects and repositories, I wanted to compile them all into a single project. Not only do these projects offer references to learn from when creating my next game, having them included with ADK allows me to share code between the projects. The idea is to create systems and game features that are reusable across projects, so that, for example, if I program an A* pathfinding system in one game I will have it available to use in another game.

<img src="https://media3.giphy.com/media/P8blTBGB0RCX5EJLO3/giphy.gif" alt="ayax gif" width="854" height="480">

If I was going to create my own custom C++ game engine, I needed it to be better than the other engines available. The goal was to make an engine that was more flexible than Unreal engine and better than Game Maker.

## Features
- **Prefabs**
  - Prefab system allows you to create, configure, and store an object complete with all its property values, animations, and collisions as a reusable Asset. The Prefab Asset acts as a template from which you can create new Prefab instances in the Scene.
- **Sophisticated Integrated Editor**
  - Save and Load Levels
  - Integrated editor allows for working very close to the final product compared to using an external editor
  - Directly placing entities (which is possible because the editor is integrated) means levels aren't confined to a grid
  - Configure sprites, collisions, and animations all within the editor
- **Code Reflection**
  - Expose class fields to the editor. This is very useful in level creation; for example you can change the speed of certain enemies from within the editor. This means different instances of the same enemy class can have varying speeds. Reflection is very powerful.
- **Animation System**
  - Easy to configure multiple animations from a single spritesheet
- **Collisions**
  - Included code examples demonstrate how to easily implement pixel-perfect collision using the provided collision system
- **Timers**
  - Timers allow you to set a timer for a callback function/delegate. For example, when the player dies, you could set a 2 second timer for a function that restarts the level. Very useful.
- **Entity and Scene System**
  - What do you expect? It's a game engine
- **Entity Layers**
  - Entities can have variable depth, allowing for parallax effects and other cool tricks
- **Reference Counted Resource Management**
  - Automatic reference counting ensures resources are acquired when needed and freed when they are not

<img src="https://i.imgur.com/Fl24SvW.png" alt="ayax screenshot 3" width="854" height="480">

## Planned Features
- TAGS!!!
- Timelines/Coroutines (https://docs.unity3d.com/Manual/Coroutines.html)
- Input Manager (to handle multiple keybinds)
- GameInstance class for objects that persist throughout the entire game
- Sound FX & Music system
- Particle system
- Sample GLSL shaders
- Editor improvements


## PrOGress
### Dev Logs
[dev log 1](https://www.youtube.com/watch?v=1ZRrF1_dG0k)

[dev log 2](https://www.youtube.com/watch?v=EzYcopM69Xs)

[dev log 3 - collisions, save/load levels, editor QoL](https://www.youtube.com/watch?v=4mECFvIHDDg)

[dev log 4 - platformer showcase](https://www.youtube.com/watch?v=YY4cDHtUP5A)

[dev log 5 - Improved Engine Workflow, Platformer Game Progress, Camera Class](https://youtu.be/AYnVYH03Bl0)

### Screenshots
