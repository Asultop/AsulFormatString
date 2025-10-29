# AsulFormatString

[README.md (中文) | README_en.md (English)]
f-string f-format f-print support for plain c++ 


## 项目简介

AsulFormatString 是一个轻量的 C++ 头文件库，用来扩展字符串格式化与打印能力，提供：

- 变体参数支持（VariantType：int、double、std::string、bool、char）。
- 可注册的字符串适配器（formatAdapter）、标签适配器（labelAdapter）和函数适配器（funcAdapter）。
- 额外的 `[[...]]` 指令用于对齐、宽度、精度、浮点格式等控制。

该仓库包含：`AsulFormatString.h`（核心实现）和若干示例/测试（例如 `example.cpp`）。

## 快速开始（Windows + g++/PowerShell）

1. 在项目根目录下编译示例：

```powershell
g++ -std=c++17 -Wall -Wextra -O2 .\example.cpp -o .\example.exe
```

2. 运行示例：

```powershell
.\example.exe
```

> 注意：Windows 终端对 ANSI 颜色序列的支持视终端而定（Windows Terminal、ConHost 在较新版本的 Windows 中支持）。

## 主要类型与 API

- VariantType: 使用 std::variant 表示支持的参数类型

```cpp
using VariantType = std::variant<int, double, std::string, bool, char>;
```

- 适配器映射类型：

```cpp
using AdapterMap = std::unordered_map<std::string, std::string>;
using FuncMap = std::unordered_map<std::string, std::function<std::string(const VariantType&)>>;
```

主要方法（示例）：

- `installFormatAdapter(const AdapterMap&)`：注册 `{NAME}` -> 替换字符串模板（例如颜色模板）
- `installLabelAdapter(const AdapterMap&)`：注册 `(NAME)` -> 替换字符串（短标签）
- `installFuncFormatAdapter(const FuncMap&)`：注册 `{FUNCNAME}` -> 函数（见下面的 funcAdapter 说明）
- `installColorFormatAdapter()`：安装一组内置颜色格式
- `f(fmt, args...)`：返回格式化字符串
- `print(fmt, args...)`：直接输出格式化后的字符串

## 格式语法要点

- `{}`：消耗下一个参数并按 VariantType 转为字符串（支持格式修饰符，如宽度、精度等）。
- `{NAME}`：优先视为 funcAdapter 的函数名（若存在则消费下一个参数并调用）；否则回退为 formatAdapter 的模板替换，若两者均不存在则保留原样 `{NAME}`。
- `(NAME)`：由 labelAdapter 替换，用于短标签（例如 `(SUCCESS)`）。
- `[[...]]`：控制指令，支持：
	- `SETW:n`：设置宽度
	- `FILL:c`：设置填充字符
	- `PREC:n`：设置精度
	- `LEFT` / `RIGHT`：对齐
	- `FIXED` / `SCIENTIFIC`：浮点表示
	- `RESET`：重置格式
	- `ENDL`：输出换行

- 转义：`{{` 输出 `{`，`((` 输出 `(`。

## funcAdapter（函数适配器）

`funcAdapter` 允许你将 `{FUNCNAME}` 绑定到一个函数，函数签名为：

```cpp
std::function<std::string(const VariantType&)>
```

使用规则：
- 当格式字符串遇到 `{funcName}` 且 `funcAdapter` 注册了 `funcName` 时，会从参数队列中取出下一个 `VariantType` 参数并传入该函数，函数返回的 `std::string` 会插入输出。
- 如果没有足够的参数，会抛出 `std::invalid_argument`（你可以修改实现以改为插入空字符串或占位符）。
- 如果你希望函数不消费参数，请在 lambda 内忽略参数（仍然需要声明一个 VariantType 参数），或者我可以为你扩展支持无参函数形式。

示例（摘自 `example.cpp`）：

```cpp
// 注册 toUpper
using VT = AsulFormatString::VariantType;
AsulFormatString::FuncMap fm;
fm["toUpper"] = [](const VT &v)->std::string {
		std::string s = std::visit([](auto&& x)->std::string{
				using T = std::decay_t<decltype(x)>;
				if constexpr (std::is_same_v<T, std::string>) return x;
				else if constexpr (std::is_same_v<T, char>) return std::string(1, x);
				else return std::to_string(x);
		}, v);
		for (auto &c : s) c = (char)std::toupper((unsigned char)c);
		return s;
};
asul_formatter().installFuncFormatAdapter(fm);

// 调用示例
std::string s = f("{toUpper}", std::string("hello")); // -> "HELLO"
print("Number example: {toUpper}[[ENDL]]", 123);           // -> "123"
```

## 示例说明

请查看 `example.cpp`，其中包含：

- 颜色适配器（`installColorFormatAdapter()`）示例
- 标签适配器（`installResetLabelAdapter()`、`installLogLabelAdapter()`、`installAskLabelAdapter()`）
- `toUpper` funcAdapter 的注册和使用示例

## 开发与调试

- `AsulFormatString.h` 中有一个非模板 `f(std::string, std::string)` 用于设置内部 ANSI 颜色状态。为避免模板调用与非模板重载的二义性，库内部使用 `asul_formatter().template f<Args...>(fmt, args...)` 的形式调用成员模板。
- 当前实现会让 funcAdapter 消耗参数（设计如此）。如果你更希望函数不消费参数或有更复杂的参数传递规则，我可以修改实现并添加配置选项。

## 贡献与许可证

该项目使用 MIT 许可证（参见 `LICENSE`）。欢迎 Fork、提交 Issue 或 Pull Request。

---
