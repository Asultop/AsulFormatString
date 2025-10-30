#define ALLOW_DEBUG_ASULFORMATSTRING // 启用调试信息
#include "AsulFormatString.h"
#include <iostream>
#include <cmath>
#include <cctype>
#include <thread>
#include <chrono>
#include <future>


struct PrintStruct {
    int a;
    double b;
    std::string c;
    operator std::string() const {
        return "{a=" + std::to_string(a) + ", b=" + std::to_string(b) + ", c=" + c + "}";
    }
};
std::string stringWithRanbowColor(std::string src,double frequency=0.3){
    std::string result;
    size_t len=src.length();
    for(size_t i=0;i<len;++i){
        unsigned char r = static_cast<unsigned char>((std::sin(frequency * i + 0) + 1) * 127.5);
        unsigned char g = static_cast<unsigned char>((std::sin(frequency * i + 2) + 1) * 127.5);
        unsigned char b = static_cast<unsigned char>((std::sin(frequency * i + 4) + 1) * 127.5);
        result += Color256::rgba(r, g, b, 1).toANSI256() + src[i];
    }
    result += "\033[0m";
    return result;
}
std::string stringWithRanbowColorFrame(std::string src,int frame,double frequency=0.3){
    std::string result;
    size_t len=src.length();
    for(size_t i=0;i<len;++i){
        if(src[i]==' ') {
            result += src[i];
            continue;
        }
        unsigned char r = static_cast<unsigned char>((std::sin(frequency * i + frame * 0.3 + 0) + 1) * 127.5);
        unsigned char g = static_cast<unsigned char>((std::sin(frequency * i + frame * 0.3 + 2) + 1) * 127.5);
        unsigned char b = static_cast<unsigned char>((std::sin(frequency * i + frame * 0.3 + 4) + 1) * 127.5);
        result += Color256::rgba(r, g, b, 1).toANSI256() + src[i];
    }
    result += "\033[0m";
    return result;
}
int main(int argc,char *argv[]){
    asul_formatter().installColorFormatAdapter();
    asul_formatter().installColorFormatAdapter(); // 测试重复安装

    asul_formatter().installCursorControlLabelAdapter();
    asul_formatter().installResetLabelAdapter();
    asul_formatter().installLogLabelAdapter();
    asul_formatter().installAskLabelAdapter();

    // 注册示例 funcAdapter: toUpper
    using VT = AsulFormatString::VariantType;
    AsulFormatString::FuncMap fm;
    fm["toUpper"] = [](const VT &v)->std::string {
        std::string s = AsulFormatString::variantToString(v);
        for (auto &c : s) c = (char)std::toupper((unsigned char)c);
        return s;
    };
    fm["stringWithRainbowColor"] = [](const VT &v)->std::string {
        std::string s = AsulFormatString::variantToString(v);
        return stringWithRanbowColor(s);
    };
    asul_formatter().installFuncFormatAdapter(fm);

    // 示例：在 f/print 中调用 {toUpper}
    std::cout << f("Func f() -> {toUpper}", std::string("hello")) << std::endl;
    print("Func print() -> {toUpper}[[ENDL]]", std::string("world"));
    print("Number example: {toUpper}[[ENDL]]", 123);

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
    
    print("{} {} (RESET)[[ENDL]]",
        Color256::rgba(128,0,128,1).toANSI256()+
        Color256::rgba(255, 255, 255, 1).toANSIBackground256(),
        "Custom Purple Text"
    ); // 测试自定义颜色适配器

    //Rainbow Test
    // for(int i=0;i<=255;++i){
    //     print("{}{} ",
    //         Color256::rgba(i,0,255 - i,1).toANSI256(),
    //         // Color256::rgba(255 - i,i,0,1).toANSIBackground256(),
    //         f("R:{} G:{} B:{}",i,0,255 - i)
    //     );
    // }
    print("{}[[ENDL]]",stringWithRanbowColor("This is a rainbow colored string!")); // 彩虹文字测试
    print("Rainbow Number: {}[[ENDL]]",stringWithRanbowColor(f("{}",1.234567890))); // 彩虹文字测试
    print("{stringWithRainbowColor}[[ENDL]]", "this is also a rainbow colored string!"); // 彩虹文字测试

    for(double freq=0.1;freq<=1.0;freq+=0.1){
        print("{}[[ENDL]]",stringWithRanbowColor(f("Frequency: {}",freq),freq)); // 彩虹文字测试
    }
    
    struct SumArgs {
        int a, b, c;    
    };
    struct WrongArg{
        int a, b;
    };
    asul_formatter().installTypedFuncAdapter<SumArgs>("colorfulSum", [](SumArgs args) -> std::string {
        int sum = args.a + args.b + args.c;
        std::string s = "Sum: " + std::to_string(sum);
        return stringWithRanbowColor(s);
    });
    print("{colorfulSum}[[ENDL]]", SumArgs{10, 20, 30}); // 彩虹多参求和测试

    // 彩虹求和测试
    try{
        print("{colorfulSum}[[ENDL]]", WrongArg{100, 200}); 
    }catch(const std::exception &e){
        print("(ERROR) Caught exception: {}[[ENDL]]", e.what());
    }

    //流动彩虹
    struct RainbowArgs {
        std::string arg1;
        int frame;
    };
    asul_formatter().installTypedFuncAdapter<RainbowArgs>("stringWithRainbowColor", [](RainbowArgs args) -> std::string {
        return stringWithRanbowColorFrame(args.arg1, args.frame);
    });
    std::string flowRainbow = "Flowing Rainbow Text Animation ";
    print("(CURSOR_HIDE)");
    for(int frame=0;frame<240;++frame){
        std::string line = f("{stringWithRainbowColor}", RainbowArgs{flowRainbow, frame} );
        std::cout << "\r" << line << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(24));
    }
    print("(CURSOR_SHOW)");

    


    
}