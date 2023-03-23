# yaRenderer

yaRenderer means yet another renderer

## 项目结构

`models`存放程序使用到的模型
`src`文件夹下面是项目的代码
`src/include`存放核心代码
`src/shader` 存放编写的shader，其中IShader为公共基类，不提供任何实现

## 程序运行流程

在`main.cpp`中：

+ 程序需要预先设定各项参数，如摄像机的位置等
+ 设定图片渲染的位置，即TGAImage
+ 读取模型
+ 将参数传入到`Render`类中，调用`Rendering`函数进行渲染
+ 将渲染结果保存到一张`.tga`文件中

## 进度

正在努力开发ing~
