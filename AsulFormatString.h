
/*

 *   ______     ______   ______    
 *  /\  __ \   /\  ___\ /\  ___\   
 *  \ \  __ \  \ \  __\ \ \___  \  
 *   \ \_\ \_\  \ \_\    \/\_____\ 
 *    \/_/\/_/   \/_/     \/_____/ 
 *                                 

    File        : AsulFormatString.h
    Version     : alpha-v0.1.0
    Description : AFS extend std::string 

    Copyright (c) 2025 AsulTop
    Author     : AsulTop (GitHub: AsulTop)
    Contact    : Alivn@asul.top

    MIT License
*/

#ifndef ASULFORMATSTRING_H
#define ASULFORMATSTRING_H

#ifndef ALLOW_DEBUG_ASULFORMATSTRING
#define NO_DEBUG
#endif // ALLOW_DEBUG_ASULFORMATSTRING

#include <iostream>
#include <map>
#include <unordered_map>
#include <functional>
#include <iomanip>
#include <regex>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <variant>
#include <any>
#include <type_traits>
#include <vector>
#include "Color256.h"
class AsulFormatString {
public:
    using VariantType = std::variant<int, double, std::string, bool, char, std::any>;
    using AdapterMap = std::unordered_map<std::string, std::string>;
    using FuncMap = std::unordered_map<std::string, std::function<std::string(const VariantType &)> >;
    
    static std::string variantToString(const VariantType& v) {
        std::ostringstream oss;
        if (std::holds_alternative<int>(v)) oss << std::get<int>(v);
        else if (std::holds_alternative<double>(v)) oss << std::get<double>(v);
        else if (std::holds_alternative<std::string>(v)) oss << std::get<std::string>(v);
        else if (std::holds_alternative<bool>(v)) oss << (std::get<bool>(v) ? "true" : "false");
        else if (std::holds_alternative<char>(v)) oss << std::get<char>(v);
        else if (std::holds_alternative<std::any>(v)) {
            const std::any &a = std::get<std::any>(v);
            if (!a.has_value()) {
                oss << "<empty any>";
            } else {
                try { oss << std::any_cast<std::string>(a); }
                catch (const std::bad_any_cast&) {
                    try { oss << std::any_cast<const char*>(a); }
                    catch (const std::bad_any_cast&) {
                        oss << "<any:" << a.type().name() << ">";
                    }
                }
            }
        }
        else if (std::holds_alternative<std::any>(v)) {
            const std::any &a = std::get<std::any>(v);
            if (!a.has_value()) {
                oss << "<empty any>";
            } else {
                try { oss << std::any_cast<std::string>(a); }
                catch (const std::bad_any_cast&) {
                    try { oss << std::any_cast<const char*>(a); }
                    catch (const std::bad_any_cast&) {
                        // fallback: print type name
                        oss << "<any:" << a.type().name() << ">";
                    }
                }
            }
        }
        return oss.str();
    }
    AsulFormatString() = default;
    void installFuncFormatAdapter(const FuncMap& mp) {
        for (const auto& [key, value] : mp) {
            auto it = funcAdapter.find(key);
            if (it != funcAdapter.end()) {
                #ifdef ALLOW_DEBUG_ASULFORMATSTRING
                print("(({YELLOW}) [[LEFT]][[SETW:40]]{} already exists in funcAdapter. New: [[LEFT]][[SETW:40]]{}[[ENDL]]", "Debug",f("{UNDERLINE}={}",key,"<existing function>"),f("{UNDERLINE}={}",key,"<new function>"));
                #endif
                it->second = value;
            } else {
                funcAdapter.emplace(key, value);
            }
        }
    }
    void clearFuncFormatAdapter() { funcAdapter.clear(); }

    template <typename T, typename Fn>
    void installTypedFuncAdapter(const std::string &key, Fn fn) {
        funcAdapter[key] = [fn, key](const VariantType &v) -> std::string {
            return std::visit([&](auto&& arg) -> std::string {
                using U = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<U, std::any>) {
                    try {
                        const T &t = std::any_cast<const T&>(arg);
                        return fn(t);
                    } catch (const std::bad_any_cast&) {
                        throw std::invalid_argument("Type mismatch for funcAdapter '" + key + "': bad any_cast");
                    }
                } else {
                    if constexpr (std::is_same_v<U, T>) {
                        return fn(arg);
                    } else {
                        throw std::invalid_argument("Type mismatch for funcAdapter '" + key + "'");
                    }
                }
            }, v);
        };
    }

    void installFormatAdapter(const AdapterMap& mp) {
        for (const auto& [key, value] : mp) {
            auto it = formatAdapter.find(key);
            if (it != formatAdapter.end()) {
                #ifdef ALLOW_DEBUG_ASULFORMATSTRING
                print("(({YELLOW}) [[LEFT]][[SETW:40]]{} already exists in formatAdapter. New: [[LEFT]][[SETW:40]]{}[[ENDL]]", "Debug",f("{UNDERLINE}={}",key,it->second),f("{UNDERLINE}={}",key,value));
                #endif
                it->second = value;
            } else {
                formatAdapter.emplace(key, value);
            }
        }
    }
    void clearFormatAdapter() { formatAdapter.clear(); }

    void installLabelAdapter(const AdapterMap& mp) {
        AdapterMap tempAdapter = mp;
        for(const auto& [key, value] : mp){
            if(labelAdapter.find(key) != labelAdapter.end()){
                #ifdef ALLOW_DEBUG_ASULFORMATSTRING
                print("(({YELLOW}) [[LEFT]][[SETW:40]]{} already exists in labelAdapter. New: [[LEFT]][[SETW:40]]{}[[ENDL]]", "Debug",f("{UNDERLINE}={}",key,labelAdapter[key]),f("{UNDERLINE}={}",key,value));
                #endif
                labelAdapter[key]=value;
            }
            else tempAdapter.insert({key,value});
        }
        labelAdapter.insert(tempAdapter.begin(), tempAdapter.end());
    }
    void clearLabelAdapter() { labelAdapter.clear(); }

    //color
    void installColorFormatAdapter() {
        installFormatAdapter({
            {"RED",Color256::rgba(255, 0, 0, 1).toANSI256()+"{}"+"\033[0m"},
            {"GREEN",Color256::rgba(0, 255, 0, 1).toANSI256()+"{}"+"\033[0m"},
            {"YELLOW",Color256::rgba(255, 255, 0, 1).toANSI256()+"{}"+"\033[0m"},
            {"BLUE",Color256::rgba(0, 0, 255, 1).toANSI256()+"{}"+"\033[0m"},
            {"MAGENTA",Color256::rgba(255, 0, 255, 1).toANSI256()+"{}"+"\033[0m"},
            {"CYAN",Color256::rgba(0, 255, 255, 1).toANSI256()+"{}"+"\033[0m"},
            {"LIGHT_GRAY",Color256::rgba(192, 192, 192, 1).toANSI256()+"{}"+"\033[0m"},
            {"DARK_GRAY",Color256::rgba(128, 128, 128, 1).toANSI256()+"{}"+"\033[0m"},
            {"LIGHT_RED",Color256::rgba(255, 102, 102, 1).toANSI256()+"{}"+"\033[0m"},
            {"LIGHT_GREEN",Color256::rgba(102, 255, 102, 1).toANSI256()+"{}"+"\033[0m"},
            {"LIGHT_YELLOW",Color256::rgba(255, 255, 102, 1).toANSI256()+"{}"+"\033[0m"},
            {"LIGHT_BLUE",Color256::rgba(102, 102, 255, 1).toANSI256()+"{}"+"\033[0m"},
            {"LIGHT_MAGENTA",Color256::rgba(255, 102, 255, 1).toANSI256()+"{}"+"\033[0m"},
            {"LIGHT_CYAN",Color256::rgba(102, 255, 255, 1).toANSI256()+"{}"+"\033[0m"},
            {"WHITE",Color256::rgba(255, 255, 255, 1).toANSI256()+"{}"+"\033[0m"},
            {"UNDERLINE", "\033[4m{}\033[0m"}
        });
    }

    void installResetLabelAdapter() {
        installLabelAdapter({
            {"RESET", "\033[0m"}
        });
    }
    void installCursorControlLabelAdapter(){
        installLabelAdapter({
            {"CURSOR_UP", "\033[A"},
            {"CURSOR_DOWN", "\033[B"},
            {"CURSOR_FORWARD", "\033[C"},
            {"CURSOR_BACKWARD", "\033[D"},
            {"CURSOR_SAVE_POS", "\0337"},
            {"CURSOR_RESTORE_POS", "\0338"},
            {"CURSOR_HIDE", "\033[?25l"},
            {"CURSOR_SHOW", "\033[?25h"}
        });
    }
    void installLogLabelAdapter() {
        installLabelAdapter({
            {"SUCCESS", f("{GREEN}"     , "[Success]")},
            {"INFO"   , f("{LIGHT_BLUE}", "[Info===]")},
            {"WARN"   , f("{YELLOW}"    , "[Warn===]")},
            {"ERROR"  , f("{RED}"       , "[Error==]")}
        });
    }

    void installAskLabelAdapter(){
        installLabelAdapter({
            {"ASK_Y",f("({UNDERLINE}/N)","Y")},
            {"ASK_N",f("(Y/{UNDERLINE})","N")}
        });
    }
    // AsulFormatString
    AsulFormatString* f(std::string ansi256, std::string ansiBackground256) {
        ANSI256 = ansi256;
        ANSIBackground256 = ansiBackground256;
        return this;
    }

    template <typename... Args>
    std::string f(std::string fmt, const Args&... args) {
        std::vector<VariantType> argsVec;
        argvs_helper(argsVec, args...);
        
        if (!hasValidParentheses(fmt)) { 
            ANSI256 = "";
            ANSIBackground256 = "";
            throw std::invalid_argument("Mismatched parentheses in format string");
        } 
        if (!hasValidCurlyBraces(fmt)) {
            ANSI256 = "";
            ANSIBackground256 = "";
            throw std::invalid_argument("Mismatched curly braces in format string");
        }
        if (!hasValidSquareBrackets(fmt)) {
            ANSI256 = "";
            ANSIBackground256 = "";
            throw std::invalid_argument("Mismatched square brackets in format string");
        }

        std::string tempFmt = processLabelAdapter(fmt);
        std::string processedFmt = processAdapter(tempFmt);
        std::string result;
        size_t argIndex = 0, i = 0, len = processedFmt.length();
        while (i < len) {
            if (processedFmt[i] == '{') {
                if (i + 1 < len && processedFmt[i + 1] == '{') {
                    result += '{';
                    i += 2;
                    continue;
                }
            }
            if (processedFmt[i] == '(') {
                if (i + 1 < len && processedFmt[i + 1] == '(') {
                    result += '(';
                    i += 2;
                    continue;
                }
            }

            if (processedFmt[i] == '{') {
                size_t j = processedFmt.find('}', i);
                if (j == std::string::npos) throw std::invalid_argument("Unclosed curly brace in format string");
                std::string placeholder = processedFmt.substr(i + 1, j - i - 1);
                if (placeholder.empty()) {
                    if (argIndex < argsVec.size()) {
                        result += variantToString(argsVec[argIndex]);
                        argIndex++;
                    } else {
                        result += "{}";
                    }
                } else {
                    // 如果是已注册的 funcAdapter（函数格式化适配器），用下一个参数调用并插入返回值
                    auto itF = funcAdapter.find(placeholder);
                    if (itF != funcAdapter.end()) {
                        if (argIndex < argsVec.size()) {
                            result += itF->second(argsVec[argIndex]);
                            ++argIndex;
                        } else {
                            throw std::invalid_argument(std::string("Not enough arguments for function format '{") + placeholder + "}'");
                        }
                    } else {
                        result += processedFmt.substr(i, j - i + 1);
                    }
                }
                i = j + 1;
            } else {
                result += processedFmt[i];
                i++;
            }
        }
        ANSI256 = "";
        ANSIBackground256 = "";
        return result;
    }
    void print(const std::string& fmt) {
        print(fmt,"");
    }
    template <typename... Args>
    void print(const std::string& fmt, const Args&... args) {
        std::vector<VariantType> argsVec;
        argvs_helper(argsVec, args...);

        if (!hasValidParentheses(fmt)) {
            ANSI256 = "";
            ANSIBackground256 = "";
            throw std::invalid_argument("Mismatched parentheses in format string");
        }
        if (!hasValidCurlyBraces(fmt)) {
            ANSI256 = "";
            ANSIBackground256 = "";
            throw std::invalid_argument("Mismatched curly braces in format string");
        }
        if (!hasValidSquareBrackets(fmt)) {
            ANSI256 = "";
            ANSIBackground256 = "";
            throw std::invalid_argument("Mismatched square brackets in format string");
        }

        std::string work = fmt;
        FormatState fs;
        size_t argIndex = 0;
        std::string output;
        auto processInnerInToken = [&](const std::string &tokenSrc) {
            std::string innerWork = tokenSrc;
            for (size_t k = 0; k < innerWork.size();) {
                char ch = innerWork[k];
                if (ch == '{' && k + 1 < innerWork.size() && innerWork[k + 1] == '{') {
                    innerWork = innerWork.substr(0, k) + "{" + innerWork.substr(k + 2);
                    ++k;
                    continue;
                }
                if (ch == '(' && k + 1 < innerWork.size() && innerWork[k + 1] == '(') {
                    innerWork = innerWork.substr(0, k) + "(" + innerWork.substr(k + 2);
                    ++k;
                    continue;
                }
                if (ch == '{') {
                    size_t kk = innerWork.find('}', k);
                    if (kk == std::string::npos) {
                        throw std::invalid_argument(std::string("Unclosed '{' inside [[]] token: [[") + tokenSrc + "]]" );
                    }
                    std::string inner = innerWork.substr(k + 1, kk - k - 1);
                    if (inner.empty()) {
                        if (argIndex >= argsVec.size()) {
                            throw std::invalid_argument(std::string("Not enough arguments for {} inside [[]] token: [[") + tokenSrc + "]]" );
                        }
                        std::string rep = variantToString(argsVec[argIndex]);
                        ++argIndex;
                        innerWork = innerWork.substr(0, k) + rep + innerWork.substr(kk + 1);
                        continue;
                    } else {
                        if (inner.find('[') != std::string::npos || inner.find('(') != std::string::npos) {
                            k = kk + 1;
                            continue;
                        }
                        auto itFA = formatAdapter.find(inner);
                        if (itFA != formatAdapter.end()) {
                            innerWork = innerWork.substr(0, k) + itFA->second + innerWork.substr(kk + 1);
                            continue;
                        } else {
                            throw std::invalid_argument(std::string("Unknown format adapter '{") + inner + "}' inside [[]] token: [[" + tokenSrc + "]]" );
                        }
                    }
                }
                if (ch == '(') {
                    size_t kk = innerWork.find(')', k);
                    if (kk == std::string::npos) {
                        throw std::invalid_argument(std::string("Unclosed '(' inside [[]] token: [[") + tokenSrc + "]]" );
                    }
                    std::string name = innerWork.substr(k + 1, kk - k - 1);
                    auto it = labelAdapter.find(name);
                    if (it != labelAdapter.end()) {
                        innerWork = innerWork.substr(0, k) + it->second + innerWork.substr(kk + 1);
                        continue;
                    } else {
                        throw std::invalid_argument(std::string("Unknown label '(") + name + ")' inside [[]] token: [[" + tokenSrc + "]]" );
                    }
                }
                ++k;
            }
            return innerWork;
        };

        for (size_t i = 0; i < work.size(); ) {
            char c = work[i];
            if (c == '{' && i + 1 < work.size() && work[i + 1] == '{') {
                output += '{';
                i += 2;
                continue;
            }
            if (c == '(' && i + 1 < work.size() && work[i + 1] == '(') {
                output += '(';
                i += 2;
                continue;
            }
            if (c == '[') {
                if (i + 1 < work.size() && work[i + 1] == '[') {
                    size_t j = work.find("]]", i + 2);
                    if (j == std::string::npos) {
                        bool looksLikeCSI = false;
                        size_t lookEnd = std::min(work.size(), i + (size_t)20);
                        for (size_t kk = i + 1; kk < lookEnd; ++kk) {
                            unsigned char ch2 = static_cast<unsigned char>(work[kk]);
                            if ((ch2 >= 'A' && ch2 <= 'Z') || (ch2 >= 'a' && ch2 <= 'z')) { looksLikeCSI = true; break; }
                        }
                        if (looksLikeCSI) {
                            output += '[';
                            ++i;
                            continue;
                        }

                        std::string tail = work.substr(i, std::min<size_t>(80, work.size() - i));
                        std::ostringstream _oss;
                        _oss << "Unclosed '[[' in format string at pos " << i << ": '";
                        for (unsigned char ch : tail) {
                            if (ch >= 32 && ch <= 126) _oss << ch;
                            else { _oss << "\\x" << std::hex << std::uppercase << (int)ch << std::dec; }
                        }
                        _oss << "'";
                        throw std::invalid_argument(_oss.str());
                    }
                    std::string token = work.substr(i + 2, j - (i + 2));
                    if (token.empty()) {
                        throw std::invalid_argument("Empty [[]] directive is not allowed");
                    }
                    token = processInnerInToken(token);
                    if (token == "LEFT") { fs.left = true; fs.right = false; }
                    else if (token == "RIGHT") { fs.right = true; fs.left = false; }
                    else if (token == "RESET") { fs.reset(); }
                    else if (token == "FIXED") { fs.fixedFmt = true; fs.scientificFmt = false; }
                    else if (token == "SCIENTIFIC") { fs.scientificFmt = true; fs.fixedFmt = false; }
                    else if (token == "ENDL") { output += '\n'; }
                    else {
                        auto pos = token.find(':');
                        if (pos != std::string::npos) {
                            std::string key = token.substr(0, pos);
                            std::string val = token.substr(pos + 1);
                            if (key == "SETW") {
                                if (val.empty()) throw std::invalid_argument("SETW requires a numeric value inside [[]]");
                                try { fs.width = std::stoi(val); fs.widthTemp = true; } catch(...) { throw std::invalid_argument("Invalid integer for SETW inside [[]]: '" + val + "'"); }
                            } else if (key == "FILL") {
                                if (val.empty()) throw std::invalid_argument("FILL requires a character inside [[]]");
                                fs.fillChar = val[0];
                            } else if (key == "PREC") {
                                if (val.empty()) throw std::invalid_argument("PREC requires a numeric value inside [[]]");
                                try { fs.precision = std::stoi(val); } catch(...) { throw std::invalid_argument("Invalid integer for PREC inside [[]]: '" + val + "'"); }
                            } else {
                                throw std::invalid_argument(std::string("Unknown [[]] directive: [[") + token + "]]" );
                            }
                        } else {
                            throw std::invalid_argument(std::string("Unknown [[]] directive: [[") + token + "]]" );
                        }
                    }
                    i = j + 2;
                    continue;
                } else {
                    output += '[';
                    ++i;
                    continue;
                }
            }

            if (c == '(') {
                size_t j = work.find(')', i);
                if (j == std::string::npos) {
                    output += '(';
                    ++i;
                    continue;
                }
                std::string name = work.substr(i + 1, j - i - 1);
                auto it = labelAdapter.find(name);
                if (it != labelAdapter.end()) {
                    work = work.substr(0, i) + it->second + work.substr(j + 1);
                    continue;
                } else {
                    output += work.substr(i, j - i + 1);
                    i = j + 1;
                    continue;
                }
            }

            if (c == '{') {
                size_t j = work.find('}', i);
                if (j == std::string::npos) {
                    output += '{';
                    ++i;
                    if (fs.widthTemp) { fs.width = 0; fs.widthTemp = false; }
                    continue;
                }
                std::string inner = work.substr(i + 1, j - i - 1);
                if (inner.empty()) {
                    if (argIndex < argsVec.size()) {
                        output += formatVariantWithModifiers(argsVec[argIndex], fs);
                        ++argIndex;
                    } else {
                        output += "{}";
                    }
                    i = j + 1;
                    continue;
                } else {
                    if (inner.find('[') != std::string::npos || inner.find('(') != std::string::npos) {
                        output += work.substr(i, j - i + 1);
                        i = j + 1;
                        continue;
                    }
                    // 如果注册了 funcAdapter，则以下一个参数调用并把返回值写入输出
                    auto itF = funcAdapter.find(inner);
                    if (itF != funcAdapter.end()) {
                        if (argIndex < argsVec.size()) {
                            output += itF->second(argsVec[argIndex]);
                            ++argIndex;
                            i = j + 1;
                            continue;
                        } else {
                            throw std::invalid_argument(std::string("Not enough arguments for function format '{") + inner + "}'");
                        }
                    }
                    auto itFA = formatAdapter.find(inner);
                    if (itFA != formatAdapter.end()) {
                        work = work.substr(0, i) + itFA->second + work.substr(j + 1);
                        continue;
                    } else {
                        output += work.substr(i, j - i + 1);
                        i = j + 1;
                        continue;
                    }
                }
            }

            output += c;
            ++i;
            if (fs.widthTemp) { fs.width = 0; fs.widthTemp = false; }
        }

        ANSI256 = "";
        ANSIBackground256 = "";
        std::cout << output;
    }

private:
    AdapterMap formatAdapter;
    AdapterMap labelAdapter;
        FuncMap funcAdapter;


    std::string ANSI256="", ANSIBackground256="";

    void argvs_helper(std::vector<VariantType>& vec) { /* no args */ }
    // trait to check if T is streamable to ostream
    template <typename T>
    struct is_streamable {
        template <typename U>
        static auto test(int) -> decltype(std::declval<std::ostream&>() << std::declval<U>(), std::true_type());
        template <typename>
        static auto test(...) -> std::false_type;
        static constexpr bool value = decltype(test<T>(0))::value;
    };

    template <typename T>
    void argvs_helper(std::vector<VariantType>& vec, const T& first) {
        using DT = std::decay_t<T>;
        if constexpr (std::is_same_v<DT, int> || std::is_same_v<DT, double> || std::is_same_v<DT, std::string> || std::is_same_v<DT, bool> || std::is_same_v<DT, char> || std::is_same_v<DT, std::any>) {
            vec.push_back(first);
        } else if constexpr (std::is_same_v<DT, const char*> || std::is_same_v<DT, char*>) {
            vec.push_back(std::string(first));
        } else if constexpr (is_streamable<DT>::value) {
            std::ostringstream oss; oss << first; vec.push_back(oss.str());
        } else {
            // unknown/non-streamable custom type: store as std::any so installTypedFuncAdapter can any_cast it
            vec.push_back(std::any(first));
        }
    }
    template <typename T, typename... Args>
    void argvs_helper(std::vector<VariantType>& vec, const T& first, const Args&... args) {
        // Use the single-argument overload to correctly handle conversion/any storage
        argvs_helper(vec, first);
        argvs_helper(vec, args...);
    }

    bool hasValidCurlyBraces(const std::string& s) {
        std::stack<char> p;
        for (size_t i = 0; i < s.size(); ++i) {
            char c = s[i];
            if (c == '{') {
                if (i + 1 < s.size() && s[i + 1] == '{') { ++i; continue; }
                p.push('{');
            } else if (c == '}') {
                if (i + 1 < s.size() && s[i + 1] == '}') { ++i; continue; }
                if (!p.empty() && p.top() == '{') {
                    p.pop();
                }
            }
        }
        return p.empty();
    }

    bool hasValidParentheses(const std::string& s) {
        std::stack<char> p;
        for (size_t i = 0; i < s.size(); ++i) {
            char c = s[i];
            if (c == '(') {
                if (i + 1 < s.size() && s[i + 1] == '(') { ++i; continue; }
                p.push('(');
            } else if (c == ')') {
                if (i + 1 < s.size() && s[i + 1] == ')') { ++i; continue; }
                if (!p.empty() && p.top() == '(') {
                    p.pop();
                }
            }
        }
        return p.empty();
    }

    bool hasValidSquareBrackets(const std::string& s) {
        std::stack<char> p;
        for (size_t i = 0; i < s.size(); ++i) {
            unsigned char c = static_cast<unsigned char>(s[i]);
            if (c == '[') {
                if (i > 0 && static_cast<unsigned char>(s[i - 1]) == 27) continue;
                if (i + 1 < s.size() && s[i + 1] == '[') {
                    p.push('[');
                    ++i;
                } else {
                    continue;
                }
            } else if (c == ']') {
                if (i + 1 < s.size() && s[i + 1] == ']') {
                    if (p.empty() || p.top() != '[') return false;
                    p.pop();
                    ++i;
                } else {
                    continue;
                }
            }
        }
        return p.empty();
    }

    std::string processAdapter(const std::string& fmt) {
        std::string processed = fmt;
        std::regex pattern(R"(\{(\w+)\})");
        std::smatch match;
        std::string temp = processed;
        
        processed.clear();
        while (std::regex_search(temp, match, pattern)) {
            processed += temp.substr(0, match.position());
            std::string placeholder = match[1];
            if (formatAdapter.find(placeholder) != formatAdapter.end()) {
                processed += formatAdapter[placeholder];
            } else {
                processed += match[0];
            }
            
            temp = match.suffix();
        }
        processed += temp;
        
        return processed;
    }

    std::string processLabelAdapter(const std::string& fmt) {
        std::string processed = fmt;
        std::regex pattern(R"(\((\w+)\))");
        std::smatch match;
        std::string temp = processed;
        
        processed.clear();
        while (std::regex_search(temp, match, pattern)) {
            processed += temp.substr(0, match.position());
            std::string placeholder = match[1];
            if (labelAdapter.find(placeholder) != labelAdapter.end()) {
                processed += labelAdapter[placeholder];
            } else {
                processed += match[0];
            }
            temp = match.suffix();
        }
        processed += temp;
        return processed;
    }

    
    struct FormatState {
        bool left = false;
        bool right = false;
        int width = 0;
        bool widthTemp = false;
        char fillChar = ' ';
        int precision = -1;
        bool fixedFmt = false;
        bool scientificFmt = false;
        void reset() {
            left = right = false;
            width = 0; widthTemp = false;
            fillChar = ' ';
            precision = -1;
            fixedFmt = scientificFmt = false;
        }
    };
    std::string formatVariantWithModifiers(const VariantType& v, FormatState &fs) {
        std::ostringstream oss;
        if (fs.fillChar != ' ') oss << std::setfill(fs.fillChar);
        if (fs.precision >= 0) oss << std::setprecision(fs.precision);
        if (fs.fixedFmt) oss << std::fixed;
        if (fs.scientificFmt) oss << std::scientific;
        if (fs.left) oss << std::left;
        if (fs.right) oss << std::right;
        if (fs.width > 0) oss << std::setw(fs.width);

        if (std::holds_alternative<int>(v)) oss << std::get<int>(v);
        else if (std::holds_alternative<double>(v)) oss << std::get<double>(v);
        else if (std::holds_alternative<std::string>(v)) oss << std::get<std::string>(v);
        else if (std::holds_alternative<bool>(v)) oss << (std::get<bool>(v) ? "true" : "false");
        else if (std::holds_alternative<char>(v)) oss << std::get<char>(v);

        if (fs.widthTemp) { fs.width = 0; fs.widthTemp = false; }

        return oss.str();
    }
};

inline AsulFormatString &asul_formatter() {
  static AsulFormatString inst;
  return inst;
}

template <typename... Args>
inline std::string f(const std::string &fmt, const Args &...args) {
    return asul_formatter().template f<Args...>(fmt, args...);
}

inline void print(const std::string &fmt) { asul_formatter().print(fmt); }
template <typename... Args>
inline void print(const std::string &fmt, const Args &...args) {
    asul_formatter().print(fmt, args...);
}

#endif // ASULFORMATSTRING_H
    