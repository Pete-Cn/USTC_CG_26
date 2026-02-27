## 1. 如何编译

mingw是单配置生成器，需要在构建阶段设置属性

正确方法（在build文件夹下）：

```
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release 
cmake --build .
```

错误：

```
cmake .. -G "MinGW Makefiles"
cmake --build . --config RELEASE
```

单配置生成器必须在cmake的时候就确定编译模式

