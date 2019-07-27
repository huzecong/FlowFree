# FlowFree

This repository holds FlowFree, a Qt port of the mobile game [Flow Free](https://www.bigduckgames.com/flowfree).

The code is the product of a summer semester course project.

This project uses [qml-material](https://github.com/papyros/qml-material), the Material Design UI for Qt QML by @papyros.

# Description

FlowFree is a game connecting dots by pipes. No two pipes can intersect or overlap.

You can get a glimpse of the game's mechanics through the GIF below:
![intro-1](https://raw.githubusercontent.com/huzecong/FlowFree/master/doc/image/intro-1.gif)

The game also features an auto-solve function, implementing a fast solving algorithm. You can see it in action below:
![intro-2](https://raw.githubusercontent.com/huzecong/FlowFree/master/doc/image/intro-2.gif)

The algorithm is a simple extension of what is known as "connectedness-based state compression dynamic programming" (abbreviated "plug-DP") in the Chinese community of Olympiad in Informatics contestants. It is the algorithm behind the solution to this problem: [URAL 1519 - Formula 1](http://acm.timus.ru/problem.aspx?space=1&num=1519).
A [keynote slide](https://github.com/huzecong/FlowFree/blob/master/solver/FlowFree%E6%B1%82%E8%A7%A3%E7%AE%97%E6%B3%95.key) in Chinese briefly describes the algorithm.
