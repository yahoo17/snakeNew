# snakeNew

##一、这是啥
一个用Qt5.9.8开发的简易贪吃蛇小游戏，支持单人游戏、双人游戏、联机游戏三种模式

##二、单人模式
###1）操作简介
WSAD分别对应着上下左右，方向键也可以操作，玩家通过控制键盘使蛇吃到各种各样的果子
###2）图案含义
红色代表玩家的蛇
黄色代表好果子，吃到分数+1，蛇长度+1
黑色代表坏果子，吃到分数-1，蛇长度-1，若蛇长为1，则不-1
###3）游戏终止
当蛇头碰到自己的身体或超出边线，游戏终止

##三、双人模式
###1）操作简介
WSAD分别对应着player1的上下左右，方向键代表player2的上下左右，玩家通过控制键盘使蛇吃到各种各样的果子
###2）图案含义
红色代表player1的蛇
蓝色代表player2的蛇
黄色代表好果子，吃到分数+1，蛇长度+1
黑色代表坏果子，吃到分数-1，蛇长度-1，若蛇长为1，则不-1
###3）游戏终止
当蛇头碰到自己的身体或另一玩家的身体或超出边线，游戏终止

##四、联机模式
###1）操作简介
联机模式为网络化的双人模式