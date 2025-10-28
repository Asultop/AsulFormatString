#ifndef COLOR_256_H
#define COLOR_256_H
#include <string>
#include <algorithm>
#include <sstream>
class Color256{
    public:
        Color256(unsigned int r, unsigned int g, unsigned int b): r(r), g(g), b(b) {};
        static Color256 rgba(unsigned int r, unsigned int g, unsigned int b, unsigned int a){
            Color256 color(
                std::clamp(r, 0u, 255u),
                std::clamp(g, 0u, 255u),
                std::clamp(b, 0u, 255u)
            );
            return color;
        }
        unsigned int getR() const{
            return r;
        };
        unsigned int getG() const{
            return g;
        };
        unsigned int getB() const{
            return b;
        };
        std::string toANSI256() const{
            int r6 = (r * 6) / 256;
            int g6 = (g * 6) / 256;
            int b6 = (b * 6) / 256;
            int color_index = 16 + 36 * r6 + 6 * g6 + b6;
            std::ostringstream oss;
            oss << "\033[38;5;" << color_index << "m";
            return oss.str();
        };
        std::string toANSIBackground256() const{
            int r6 = (r * 6) / 256;
            int g6 = (g * 6) / 256;
            int b6 = (b * 6) / 256;
            int color_index = 16 + 36 * r6 + 6 * g6 + b6;
            std::ostringstream oss;
            oss << "\033[48;5;" << color_index << "m";
            return oss.str();
        };
    private:
        unsigned int r, g, b;
};

#endif // COLOR_256_H