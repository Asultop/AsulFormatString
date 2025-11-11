/**
 * i18n_example.cpp
 */

#include "AsulFormatString.h"
#include <iostream>
#include <cmath>
#include <cctype>
struct RainbowArgs {
    std::string text;
    int index;
};
std::string stringWithRanbowColorFrame(RainbowArgs args){
    std::string result;
    size_t len=args.text.length();
    for(size_t i=0;i<len;++i){
        if(args.text[i]==' ') {
            result += args.text[i];
            continue;
        }
        unsigned char r = static_cast<unsigned char>((std::sin(0.3 * i + args.index * 0.3 + 0) + 1) * 127.5);
        unsigned char g = static_cast<unsigned char>((std::sin(0.3 * i + args.index * 0.3 + 2) + 1) * 127.5);
        unsigned char b = static_cast<unsigned char>((std::sin(0.3 * i + args.index * 0.3 + 4) + 1) * 127.5);
        result += Color256::rgba(r, g, b, 1).toANSI256() + args.text[i];
    }
    result += "\033[0m";
    return result;

}

int main(int argc,char *argv[]){
    asul_formatter().installColorFormatAdapter();

    asul_formatter().installCursorControlLabelAdapter();
    asul_formatter().installResetLabelAdapter();
    asul_formatter().installLogLabelAdapter();
    asul_formatter().installAskLabelAdapter();
    asul_formatter().installTypedFuncAdapter<RainbowArgs>("stringWithRainbowColor",stringWithRanbowColorFrame);

    print("Choose a language: [[ENDL]] {stringWithRainbowColor}[[ENDL]] {stringWithRainbowColor} [[ENDL]] :", RainbowArgs{"1.English",0},RainbowArgs{"2.Chinese",0});
    int choice = 0;
    std::cin >> choice;

    auto installEnglish= [&](){
        asul_formatter().installLabelAdapter({
            {"GREETING", "Hello! Welcome to the internationalized program."},
            {"FAREWELL", "Goodbye! Have a nice day."}
        });
    };
    auto installChinese= [&](){
        asul_formatter().installLabelAdapter({
            {"GREETING", "你好！欢迎使用国际化程序。"},
            {"FAREWELL", "再见！祝你有美好的一天。"}
        });
    };

    if(choice==1){
        print("(SUCCESS) You have selected English language. [[ENDL]]", "");
        installEnglish();
    }else if(choice==2){
        print("(SUCCESS) 你已选择中文语言。 [[ENDL]]", "");
        installChinese();
    }else{
        print("(ERROR) Invalid choice. Defaulting to English. [[ENDL]]", "");
        installEnglish();
        choice = 1;
    }
    print("(GREETING) [[ENDL]]");
    print("(FAREWELL) [[ENDL]]");
}