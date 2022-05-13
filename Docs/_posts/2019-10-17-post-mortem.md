---
layout: post
title: Post-Mortem
date: 2019-10-22 18:00:00
description: "Evaluation of my C++ Port"
published: true
category: Development
tags: 
- Evaluation
- low-level-programming
---

Here is the comparison between the original BASIC Game (left) and my C++ port (right).

<iframe width="360" height="275" src="{{ site.baseurl }}/assets/BASIC Gameplay.mp4" frameborder="0" allowfullscreen></iframe>
<iframe width="360" height="275" src="{{ site.baseurl }}/assets/C++ Port Gameplay.mp4" frameborder="0" allowfullscreen></iframe><br>

I'm pretty happy with the port I managed to create. I made some changes in order to slightly improve gameplay but kept to the main design and feel of the game.

My use of classes for the different game data aspects (room, actions and objects) was effective, however my main Game files became crowded pretty quickly. I improved this by adding a Map class to hold information about the rooms and objects, but I could've improved this more with a Player class to clean up my code more.

However, the game runs smoothly and I have fixed the known bugs with the game

Some main changes I made were:
* Changed "CARRYING?" to "INVENTORY"
* Changed "CLIMB ROPE" to "CLIMB TREE" since that's what people naturally assumed the command was.
* Allowed the player to chop down the tree using "SWING AXE", meaning that they could no longer climb it.
* Stopped the player from being able to relight the candle if it was burnt out. In the original game it would seem like you had relit the candle, but performs actions like the candle isn't lit.
* Stopped the player from moving back into the marsh when the boat gets stuck.
* Allows the player to get into the house from the front door, stopping the player from getting compeltely stuck when using the boat, this was game ending in the original version.
* When in the room with the bats and the ghosts, the player can leave the room and vanquish the enemies only. In the original game, these rooms were game ending if you didn't have the correct objects. Now the bats and ghosts simply block the player from progressing until they are removed.
* In order to complete the game, you have to collect all of the treasures instead of all the items. This makes more sense and doesn't change the gameplay since the player has to collect most of the objects in order to get all the treasures.
