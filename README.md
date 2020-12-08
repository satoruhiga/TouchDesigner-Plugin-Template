### Youtube Link

- [How to use TouchDesigner-Plugin-Template on Windows](https://www.youtube.com/watch?v=t_KHXpAErsA)  
- [How to use TouchDesigner-Plugin-Template on OSX](https://www.youtube.com/watch?v=_iOq9BpKSE8)

### 1. Change CMakeLists.txt as you need

```cmake
BuiltCustomOp(CustomCHOP "src/CHOP_main.cpp" "")
BuiltCustomOp(CustomSOP "src/SOP_main.cpp" "")
BuiltCustomOp(CustomTOP "src/TOP_main.cpp" "")
```

### 2. Open Project in Visual Studio 2017 or later

1. Launch Visual Studio 2019
2. `File` > `Open` > `CMake` and select `CMakeFiles.txt`

### 3. Write your own CPlusPlus Module :)

Update `src/XOP_main.cpp`
