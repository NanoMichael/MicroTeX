# cLaTeXMath

`cLaTeXMath` 是一个跨平台实时渲染的 `LaTeX` 渲染引擎，它可以被嵌入到各个平台（Android, iOS, Windows, Mac OS 等）的应用中。下面两张图展示了它运行在 Windows 和 Ubuntu 系统中的效果。

![example_windows](readme/example_windows.PNG)

![example_ubuntu](readme/example_ubuntu.png)

# 查看 Demo

项目使用 `CMake` 进行构建，首先请确保 `CMake` 已经安装

- Windows 用户建议安装 `CygWin` 或 `MinGW`，并确保 `Gdiplus` 正确安装
- Linux 用户请确保 `GTKMM` 和 `GSVMM` 已经安装

在以上步骤完成后，执行以下命令编译：

``` sh
mkdir build
cd build
cmake ..
make
```

然后在 build 目录下执行 `LaTeX` 即可。

# 如何使用

`cLaTeXMath` 使用了很多 `C++ 11` 特性，所以在开始之前，应确保编译器支持 `C++ 11` 或者 `C++ 0x`。除此之外，`cLaTeXMath` 使用 [tinyxml2](https://github.com/leethomason/tinyxml2) 来解析 xml 资源，它已经被包含在项目中，可以在目录 `xml` 下找到。

`cLaTeXMath` 已经封装好了大部分的 `TeX` 语法和宏，基本上不需要自己实现特定的宏和命令。`cLaTeXMath` 定义了自己的抽象化图形接口（在目录 `graphic` 下），在特定平台下，你必须实现这些接口。以下为这些接口的说明，都定义在 `graphic/graphic.h` 文件中

- *Font*
    这个类用来表示字体，`cLaTeXMath` 用它从文件中加载和创建字体。

- *TextLayout*
    这个类用来表示一个文字布局。当程序在解析 `TeX` 文本时，当遇到未知字符（未在程序中定义的字符集）时使用这个类来进行文字布局。

- *Graphics2D*
    这个类用来表示 2D 图形环境，里面定义了基本的绘图和仿射变换。这是绘图的基础类，`cLaTeXMath` 使用它来进行绘图。

除了以上说明的接口之外，程序还定义了 `RES_BASE` 全局变量，表示资源文件的根目录。你可以参考 `samples/cairo` 以及 `samples/gdi_win` 里面的实现。

## 如何渲染 LaTeX

要进行 LaTeX 渲染，过程非常简单。首先加载资源，然后解析 `TeX` 代码，最后绘制。以下介绍了一些关键的类。

- *LaTeX*
    定义在 `latex.h` 文件中。这个类实现了最基础的操作哦，包括**加载资源**，**释放资源**以及**解析 TeX 代码**

- *TeXRender*
    定义在 `render.h` 文件中。表示抽象的 `LaTeX` 图形，能直接在 2D 图形环境中绘制。

- *TeXRenderBuilder*
    定义在 `render.h` 文件中。这个类保存了绘制一个 `TeXRender` 的基本参数，包括宽度，字体，颜色等。

请查看 `latex.h` 和 `render.h` 以获取更多细节。

在开始绘制之前，你必须声明**资源文件的根目录**，也就是设置 `RES_BASE` 全局变量的值（默认是 `res`），然后调用 `LaTeX::init()` 函数加载资源。

然后你有两种方式来渲染一段 `TeX` 代码，以下代码（使用了 `cairo` 的实现）展示了详细的用法。

- 通用模式

``` c++
#include "latex.h"

using namespace tex;

void drawFormula() {

    // 加载资源
    LaTeX::init();

    // 声明 TeX 代码，注意必须是 wstring
    wstring ltx = L"\\text{What a beautiful day}";
    TeXRender* render = LaTeX::parse(ltx,
        // 图形环境的逻辑宽度（像素）
        720,
        // 字体大小（像素）
        23,
        // 行间距（像素）
        10,
        // argb 颜色
        0xff101010);

    Graphics2D_cairo g2d();
    // 绘制
    // 后两个参数表示左上角位置
    render->draw(g2d, 10, 10);

    // 记得删除生成的 render
    delete render;
}
```

- Builder 模式

``` c++
#include "latex.h"

using namespace tex;

void drawFormula() {

    // 加载资源
    LaTeX::init();

    TeXFormula formula;
    TeXRenderBuilder builder;

    // 声明 TeX 代码，注意必须是 wstring
    wstring ltx = L"\\text{What a beautiful day}";
    formula.setLaTeX(ltx);

    TeXRender* render = builder
        // 设置展示模式为 STYLE_DISPLAY，更多细节请参考 TeXConstants
        .setStyle(STYLE_DISPLAY)
        // 字体大小
        .setSize(23)
        // 图形环境的逻辑宽度和对齐模式
        .setWidth(UNIT_PIXEL, 720, ALIGN_LEFT)
        // 声明的图形环境的大小是否是最大宽度
        .setIsMaxWidth(false)
        // 行间距
        .setLineSpace(UNIT_PIXEL, 10)
        // 颜色
        .setForeground(0xff101010)
        // 根据以上声明的各个配置创建 render
        .build(formula);

    Graphics2D_cairo g2d();
    render->draw(10, 10);

    delete render;
}
```

应该注意的是，`Graphics2D` 在不同平台下有不同的实现。

## 调试和内存泄漏检测

`cLaTeXMath` 预定义了一些标志来帮助调试。你应该在发行版中关闭这些标志。

在标志 `__DEBUG` 开启的情况下，程序会输出一些调试信息，包括资源，符号，预定义宏等。宏 `__DBG` 和 `__log` 用来输出调试信息，具体细节请参考源码。

宏 `__GA_DEBUG` 在开启情况下，你可以在程序中使用 `\debug` 和 `\undebug` 命令来调试绘制的图形。

你可以开启 `__MEM_CHECK` 宏，然后使用 `Valgrind` 来调试内存泄漏。

以上提到的宏都定义在 `config.h` 中，你可以有选择的关闭或打开某个宏进行想要的操作。

## 资源

## 符号和宏

# 文档

[看这里](https://nanomichael.github.io/cLaTeXMath/)

# License
