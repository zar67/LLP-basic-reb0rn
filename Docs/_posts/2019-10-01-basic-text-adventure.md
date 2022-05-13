---
layout: post
title: BASIC Text Adventure
date: 2019-10-01 16:00:00
description: "Implementing a simple text adventure in BASIC"
published: true
category: Development
tags: 
- BASIC
- low-level-programming
---

<center><img src="{{ site.baseurl }}/assets/book_cover.jpg" alt="book cover" style="height: 400px;" /></center><br>

We were given the book [Write Your Own Adventure: Programs for You Microcomputer](https://www.amazon.co.uk/Write-Your-Own-Adventure-Microcomputer/dp/0686878329) in order to reproduce the text adventure inside using the BASIC implementation. This took at least 3 hours to write and fix, copying line by line of code filled with one letter variables.

My biggest mistake was not reading the little information bubbles to the side of the code that helpfully gave you information on changing the code for the type of emulator you're using or explaining how the code works.

My second mistake was just my typing ability; there were many mis-spelled variables. Specifically, when I ran the game, I encountered an error where I could go East when there wasn't an exit East. I found out that I had just simply typed in the wrong letters where the room exits were defined, so East and West were muddled up.

After a bunch of small fixes, and some reformatting of print statements for my own benefit (and sanity), the text adventure worked.

<center><img src="{{ site.baseurl }}/assets/gameplay.jpg" alt="start of gameplay" style="height: 300px;" /></center><br>

My next task is to implement this game in C++ using Object Oriented Programming and other modern coding techniques.