# SOCKS_PROXY

## 编译指南

### 编译条件

1. 需要安装vs2019，并安装其中的c/c++工具集。
2. 安装最新版cmake，并配置相关环境变量。
3. 如要重新编译三方库文件，则还需要安装python3，并配置相关环境变量。

### 编译方式

可使用两种方式进行项目的编译：
* vs图形界面操作：
    1. 打开vs2019，在Visual Studio工具栏上选择 文件 -> 打开 -> CMake，选中该项目的CMakeLists.txt
    2. 等待vs2019加载该项目完毕后，在 Visual Studio 工具栏上，有“启用集成时*CMakePresets.json*的目标系统”、“配置预设”和“生成预设”的下拉列表：![CMakePresets tools](https://learn.microsoft.com/zh-cn/cpp/build/media/target-system-dropdown.png?view=msvc-170)
    3. 在下拉列表中按照如下进行选择：![CMakePresets select](vs2019_preset.png)
    4. 在Visual Studio工具栏上选择 项目 -> 生成缓存，等待项目生成构建文件。
    5. 在Visual Studio工具栏上选择 生成 -> 全部生成，等待生成完毕后，即构建完成。
* 命令行操作：
    1. 打开命令行终端，切换到该项目的根目录路径，在其中创建build文件夹，并切换到该文件夹中。
    2. 运行下列命令进行构建：
       ```bash
       mkdir build
       cd socks_proxy/build
       # intel:
       cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CONFIGURATION_TYPES=Release -DCMAKE_C_COMPILER=icl -DCMAKE_CXX_COMPILER=icl -G Ninja
       # msvc:
       cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CONFIGURATION_TYPES=Release -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl
       # mingw:
       cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CONFIGURATION_TYPES=Release -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -G Ninja
       cmake --build . --config Release -v -j 8
       ```