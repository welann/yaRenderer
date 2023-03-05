# yaRenderer

yaRenderer means yet another renderer

## 项目结构

src文件夹下面是项目的核心代码
include文件夹下面是一些附属代码

## 一些笔记

+ 注意，这个版本（此commit）以及之前的版本都有个巨大的bug，由于由于屏幕的左下角是原点所在位置，所以默认像素的x,y坐标值都是大于0的。**但是，**当导入模型有部分不在渲染视野中时，会让它们的坐标值为负数，而此时再调用zbuffer来判断深度值时可能会导致越界！

