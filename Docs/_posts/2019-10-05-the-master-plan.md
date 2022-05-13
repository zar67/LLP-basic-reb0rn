---
layout: post
title: The Master Plan
date: 2019-10-05 10:00:00
description: "My starting plan"
published: true
category: Development
tags: 
- Planning
- Documentation
- low-level-programming
---

My first step in developing the C++ port of Basic Reb0rn was to get my documentation sorted. I setup a basic GDD and TDD with information from the book and my plans for the game.
By reading the book indepth to find out about its design, I learned alot more about the game that I did by simply copying the BASIC code. 

I outlined the structure and game flow in my documentation, these will likely change as the development of this project continues. My initial plan for the objects of the game is shown in the class diagram below.

<center><img src="{{ site.baseurl }}/assets/class_diagram.jpg" alt="class diagram for the C++ port" style="height: 400px;" /></center><br>

In my GDD I outlined what I wanted to improve about various features as they were documented. Such as:
* Using the arrow keys for movement
* The addition of a menu and game over screen
* Changing the Bats and Ghosts so that the player can leave the room the way they came in, and do that only
* Improvement of the boat and marsh

In my documentation I also laid out the words, map and objects that will be in the C++ version of the game. The map of the house can be seen below.

<center><img src="{{ site.baseurl }}/assets/house_map.jpg" alt="haunted house map" style="height: 400px;" /></center><br>

I will also be loading in the rooms, words and objects through JSON files since they are easily editable and easy to read. There is a JSON library included in the boilerplate code which allows me to get information from a JSON file. 