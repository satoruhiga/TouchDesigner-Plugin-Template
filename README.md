### 1. Change CMakeLists.txt as you need

```cmake
project(MyTestCHOP) # Change Project Name

set(OPERATOR_TYPE_CHOP 1)   # 
set(OPERATOR_TYPE_TOP 0)    # Set 1 to Change Operator Type
set(OPERATOR_TYPE_SOP 0)    #
```

### 2. Open Project in Visual Studio 2017 or later

1. Launch Visual Studio 2017
2. `File` > `Open` > `CMake` and select `CMakeFiles.txt`


### 3. Write your own CPlusPlus Module :)

Update `src/XOP_main.cpp`

### 4. Update `.vs/launch.vs.json` for Build and Run

Replace `exe` and `args` property to:

```json
      "exe": "c:\\Program Files\\Derivative\\TouchDesigner099\\bin\\TouchDesigner099.exe",
      "args": [ "${workspaceRoot}\\out\\test.toe" ],
```