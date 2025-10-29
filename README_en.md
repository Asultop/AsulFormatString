# AsulFormatString

AsulFormatString is a lightweight single-header C++ library that extends string formatting and printing capabilities. It provides template-like adapters, color support, and custom function adapters.

Main features

- Supports VariantType parameters (int, double, std::string, bool, char).
- Adapter maps for format templates (`formatAdapter`) and labels (`labelAdapter`).
- Function adapter (`funcAdapter`): bind `{FUNCNAME}` to a function with signature `std::string(const VariantType&)`. When formatting, the function consumes the next argument and inserts its returned string.
- `[[...]]` directives for alignment, width, precision, formatting, etc.
- Two interfaces: `f()` (returns std::string) and `print()` (direct output).

Files

- `AsulFormatString.h` - core implementation (header-only).
- `example.cpp` - example demonstrating color adapters, labels, and `funcAdapter` registration.
- Other tests/examples: `color256test.cpp`, `colorTest.cpp`, etc.

Quick start (Windows + g++ / PowerShell)

1. Compile the example:

```powershell
# From project root (c:\AsulTop\CXXProj\AsulFormatString)
g++ -std=c++17 -Wall -Wextra -O2 .\example.cpp -o .\example.exe
```

# AsulFormatString

AsulFormatString is a lightweight single-header C++ library that extends string formatting and printing capabilities. It provides adapter-based templates, ANSI color helpers, and a function adapter mechanism to transform argument values during formatting.

Main features

- VariantType parameter support (int, double, std::string, bool, char).
- Adapter maps for format templates (`formatAdapter`) and short labels (`labelAdapter`).
- Function adapter (`funcAdapter`) that binds `{FUNCNAME}` to `std::function<std::string(const VariantType&)>`. When formatting, the function consumes the next argument and inserts its returned string.
- `[[...]]` directives for alignment, width, precision, padding and floating formatting.
- Two interfaces: `f()` (returns std::string) and `print()` (direct output).

Files

- `AsulFormatString.h` — core implementation (header-only).
- `example.cpp` — example demonstrating color adapters, labels, and `funcAdapter` registration.
- Other tests/examples: `color256test.cpp`, `colorTest.cpp`, etc.

Quick start (Windows + g++ / PowerShell)

1. Compile the example:

```powershell
# From project root (c:\AsulTop\CXXProj\AsulFormatString)
g++ -std=c++17 -Wall -Wextra -O2 .\\example.cpp -o .\\example.exe
```

2. Run:

```powershell
.\\example.exe
```

Note: ANSI color support depends on the terminal. Windows Terminal or recent ConHost typically support VT sequences.

API overview

- Type aliases
  - `using VariantType = std::variant<int, double, std::string, bool, char>;`
  - `using AdapterMap = std::unordered_map<std::string, std::string>;`
  - `using FuncMap = std::unordered_map<std::string, std::function<std::string(const VariantType&)>>;`

- Main methods (examples)
  - `void installFormatAdapter(const AdapterMap& mp);`    // register `{NAME}` -> replacement template
  - `void clearFormatAdapter();`
  - `void installLabelAdapter(const AdapterMap& mp);`     // register `(NAME)` -> replacement string
  - `void clearLabelAdapter();`
  - `void installColorFormatAdapter();`                  // install a set of color templates
  - `void installFuncFormatAdapter(const FuncMap& mp);`  // register `{FUNCNAME}` -> function
  - `void clearFuncFormatAdapter();`
  - `std::string f(const std::string &fmt, const Args&... args);` // returns formatted string
  - `void print(const std::string &fmt, const Args&... args);`    // prints formatted output

Formatting syntax highlights

- `{}`: consumes the next argument and converts it to string according to VariantType (supports format modifiers such as width/precision).
- `{NAME}`: first checked against `funcAdapter`. If registered, it consumes the next argument and calls the function. Otherwise falls back to `formatAdapter` replacement; if neither exists, `{NAME}` is kept as-is.
- `(NAME)`: replaced using `labelAdapter` (short labels, e.g. `(SUCCESS)`).
- `[[...]]`: control directives, supports:
  - `SETW:n` - set width
  - `FILL:c` - set fill character
  - `PREC:n` - set precision
  - `LEFT` / `RIGHT` - alignment
  - `FIXED` / `SCIENTIFIC` - floating format
  - `RESET` - reset formatting
  - `ENDL` - newline

- Escaping: `{{` outputs `{`, `((` outputs `(`.

funcAdapter (function adapter)

`funcAdapter` lets you bind `{FUNCNAME}` to a function with the following signature:

```cpp
std::function<std::string(const VariantType&)>
```

Usage rules:

- When encountering `{funcName}` in the format string and `funcAdapter` has `funcName`, the formatter will consume the next `VariantType` argument and pass it to the function. The function's returned `std::string` is inserted into the output.
- If there are not enough arguments for the function, `std::invalid_argument` is thrown (you can change behavior to insert an empty string or placeholder).
- If you want the function not to consume an argument, you can still register a function that ignores its parameter; or the library can be extended to support no-arg functions.

Examples and edge cases

- Example: width and fill

```cpp
// Right aligned, width 10, fill with '*'
print("[[RIGHT]][[SETW:10]][[FILL:*]]{}[[ENDL]]", "a");
```

- Example: using formatAdapter as a color wrapper

```cpp
asul_formatter().installFormatAdapter({{"RED", "\\033[31m{}\\033[0m"}});
print("This is {RED}", "red text");
```

- Missing funcAdapter entry: `{unknown}` will remain `{unknown}` unless registered in `formatAdapter`.
- Escaping example: `{{` prints a literal `{` and `((` prints a literal `(`.

Notes on development

- `AsulFormatString.h` contains a non-template overload `f(std::string, std::string)` used internally to set ANSI state. To avoid ambiguity between template and non-template overloads, the library invokes the member template as `asul_formatter().template f<Args...>(fmt, args...)`.
- Current design makes `funcAdapter` consume an argument. If you prefer different semantics (e.g. functions that do not consume arguments), the implementation can be extended.
- Thread-safety: reading adapters after initial setup is safe; modifying adapters concurrently requires external synchronization.

Contributing & license

This project is MIT licensed (see `LICENSE`). Contributions via fork + PR or issues are welcome.

---

If you want me to also:

- overwrite the Chinese README with a full translated version (instead of adding a separate file); or
- add a CI workflow (GitHub Actions) to build the example automatically;

tell me and I'll proceed.
