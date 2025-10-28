#define ALLOW_DEBUG_ASULFORMATSTRING // 启用调试信息
#include "AsulFormatString.h"
#include <iostream>
#include <cmath>


struct PrintStruct {
    int a;
    double b;
    std::string c;
    operator std::string() const {
        return "{a=" + std::to_string(a) + ", b=" + std::to_string(b) + ", c=" + c + "}";
    }
};

int main(int argc,char *argv[]){
    asul_formatter().installColorFormatAdapter();
    asul_formatter().installColorFormatAdapter(); // 测试重复安装

    asul_formatter().installResetLabelAdapter();
    asul_formatter().installLogLabelAdapter();
    asul_formatter().installAskLabelAdapter();

    std::cout << f("(SUCCESS) {YELLOW}: {DARK_GRAY}",argv[0],"Build Passed") << std::endl;
    std::cout << f("(INFO) TestStruct: {}", PrintStruct{42, 3.14, "Example"}) << std::endl;

    print("Please confirm to continue: (ASK_Y) [[ENDL]]");                                   // 使用 ASK_Y 标签
    print("Please confirm to continue: (ASK_N) [[ENDL]]");                                   // 使用 ASK_N 标签
    print("[[LEFT]][[SETW:32]]{}|[[ENDL]]", "Column1");                                      // 测试 [[...]] 指令 [[LEFT]]
    print("[[RIGHT]][[SETW:32]]{}|[[ENDL]]", "Column2");                                     // 测试 [[...]] 指令 [[RIGHT]]
    print("[[FIXED]][[PREC:{}]]{}[[ENDL]]",7 ,3.1415926535);                                 // 测试 [[...]] 指令 [[FIXED]] 和 [[PREC]]
    print("(([]{}))[[ENDL]]");                                                               // 测试括号匹配
    print("(({YELLOW}) [[SETW:20]]{}[[ENDL]]","Yellow",f("{UNDERLINE}={}", "TestKey", "TestValue")); // 测试颜色和格式适配器
    print("(({YELLOW}) [[SETW:20]]{}      \n","Yellow",f("{UNDERLINE}={}", "TestKey", "TestValue")); // 测试颜色和格式适配器
    
    print("{} {} (RESET)",
        Color256::rgba(128,0,128,1).toANSI256()+
        Color256::rgba(255, 255, 255, 1).toANSIBackground256(),
        "Custom Purple Text"
    ); // 测试自定义颜色适配器
}