### 1. Change CMakeLists.txt as you need

```cmake
project(MyTestCHOP) # Change Project Name

set(OPERATOR_TYPE_CHOP 1)   # 
set(OPERATOR_TYPE_TOP 0)    # Set 1 to Change Operator Type
set(OPERATOR_TYPE_SOP 0)    #
```

### 2. Change `executeMode` or `begin/endGLCommands` if you want make TOP (optional)

in bottom of `main.cpp`

```cpp
#if OPERATOR_TYPE_TOP == 1

extern "C"
{
	DLLEXPORT TOP_PluginInfo GetTOPPluginInfo(void)
	{
		TOP_PluginInfo info;
		info.apiVersion = TOPCPlusPlusAPIVersion;
		info.executeMode = TOP_ExecuteMode::CPUMemWriteOnly
		//info.executeMode = TOP_ExecuteMode::OpenGL_FBO;
		return info;
	}

	DLLEXPORT TOP_CPlusPlusBase* CreateTOPInstance(const OP_NodeInfo* info, TOP_Context *context)
	{
		return new PROJECT_NAME(info, context);
	}

	DLLEXPORT void DestroyTOPInstance(TOP_CPlusPlusBase* instance, TOP_Context *context)
	{
		//context->beginGLCommands();
		delete (PROJECT_NAME*)instance;
		//context->endGLCommands();
	}
};

#endif
```

### 3. Open Project in Visual Studio 2017 or later

1. Launch Visual Studio 2017
2. `File` > `Open` > `CMake` and select `CMakeFiles.txt`


### 4. Write your own CPlusPlus Module :)

in middle of `main.cpp`

```cpp
class PROJECT_NAME : public CPLUSPLUS_BASE_CLASS
{
public:

	PROJECT_NAME(const OP_NodeInfo* info)
	{}

	virtual ~PROJECT_NAME()
	{}

	void execute(const CHOP_Output* output, OP_Inputs* inputs, void*) override
	{}

	//void execute(const TOP_OutputFormatSpecs* outputFormat, OP_Inputs* inputs, TOP_Context *context) override
	//{}

	//void execute(SOP_Output* output, OP_Inputs* inputs, void* reserved) override
	//{}

};
```

### 5. Update `.vs/launch.vs.json` for Build and Run

Replace `TestCHOP` to `MyTestCHOP` (Project Name defined at `CMakeLists.txt`)

```json
{
  "version": "0.2.1",
  "defaults": {},
  "configurations": [
    {
      "type": "dll",
      "exe": "c:\\Program Files\\Derivative\\TouchDesigner099\\bin\\TouchDesigner099.exe",
      "args": [ "${workspaceRoot}\\out\\test.toe" ],
      "project": "CMakeLists.txt",
      "projectTarget": "MyTestCHOP.dll (S:\\cmaketest\\out\\MyTestCHOP.dll)",
      "name": "MyTestCHOP.dll (S:\\cmaketest\\out\\MyTestCHOP.dll)"
    }
  ]
}
```