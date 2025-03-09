#pragma once

#include <iostream>
#include <stack>
#include <sstream>
#include <optional>
#include <variant>
#include <unordered_map>

namespace chromsole
{
        enum class ForegroundColor;
        enum class BackgroundColor;
        
        class Chromsole
        {
        public:
                template<typename T>
                Chromsole& operator<<(const T& value)
                {
                        std::string text = (std::ostringstream {} << value).str();
                        std::stack<ForegroundColor> usedColors{};
                        
                        // Color codes format: ~COLOR, ~~ (remove last set modifier), ~R (reset colors)
                        constexpr char colorCodeSequenceStart = '~';
                        const std::string sequencesSeparator = "`";
                        constexpr const char* ANSIColorCodePrefix = "\033[";
                        
                        // Surround color codes
                        for (const auto& [sequence, colorValueVariant] : colorMap) {
                                std::string pSequence = colorCodeSequenceStart + sequence;
                                
                                size_t pos{};
                                while ((pos = text.find(pSequence, pos)) != std::string::npos) {
                                        text.insert(pos, sequencesSeparator);
                                        pos += pSequence.length() + sequencesSeparator.length();
                                        text.insert(pos, sequencesSeparator);
                                        pos += sequencesSeparator.length();
                                }
                        }
                        
                        // Surround double-tilde's
                        {
                                std::string pSequence = colorCodeSequenceStart + std::string("~");
                                
                                size_t pos{};
                                while ((pos = text.find(pSequence, pos)) != std::string::npos) {
                                        text.insert(pos, sequencesSeparator);
                                        pos += pSequence.length() + sequencesSeparator.length();
                                        text.insert(pos, sequencesSeparator);
                                        pos += sequencesSeparator.length();
                                }
                        }
                        
                        // Replace content inside separators and populate stack
                        size_t pos{};
                        while (pos < text.length()) {
                                size_t startPos = text.find(sequencesSeparator, pos) + sequencesSeparator.length();
                                if (startPos == std::string::npos) break;
                                
                                size_t endPos = text.find(sequencesSeparator, startPos);
                                if (endPos == std::string::npos) break;
                                
                                std::string sequenceSubstring = text.substr(startPos, endPos - startPos);
                                
                                // Check if it is a color sequence
                                bool isColor = false;
                                for (const auto& [sequence, colorValueVariant] : colorMap) {
                                        std::string pSequence = colorCodeSequenceStart + sequence;
                                        if (sequenceSubstring == pSequence) {
                                                ForegroundColor color = std::get<ForegroundColor>(colorValueVariant);
                                                
                                                // Check if it was reset color code, clear stack
                                                if (sequence == "R")
                                                        usedColors = std::stack<ForegroundColor>();
                                                else
                                                        usedColors.push(color);
                                                
                                                std::string colorCodeStr = ANSIColorCodePrefix + std::to_string((int) color) + "m";
                                                text.replace(startPos - sequencesSeparator.length(), endPos - startPos + (2 * sequencesSeparator.length()), colorCodeStr);
                                                
                                                isColor = true;
                                                break;
                                        }
                                }
                                // Check if it is ~~
                                if (!isColor) {
                                        std::string pSequence = colorCodeSequenceStart + std::string("~");
                                        if (sequenceSubstring == pSequence) {
                                                ForegroundColor color = ForegroundColor::Reset;
                                                
                                                if (!usedColors.empty()) {
                                                        usedColors.pop();
                                                        if (!usedColors.empty()) color = usedColors.top();
                                                }
                                                
                                                std::string colorCodeStr = ANSIColorCodePrefix + std::to_string((int)color) + "m";
                                                text.replace(startPos - sequencesSeparator.length(), endPos - startPos + (2 * sequencesSeparator.length()), colorCodeStr);
                                        }
                                }
                                
                                pos = endPos+sequencesSeparator.length();
                        }
                        
                        text += (ANSIColorCodePrefix + std::to_string((int)ForegroundColor::Reset) + "m");
                        
                        std::cout << text;
                        return *this;
                }
                
                Chromsole& operator<<(std::ostream& (*func)(std::ostream&))
                {
                        func(std::cout);
                        return *this;
                }
                
                Chromsole()
                {
                        // Enable ANSI if necessary?
                }
            
        private:
                struct ConsoleFgBg
                {
                        std::optional<ForegroundColor> fg;
                        std::optional<BackgroundColor> bg;
                };
        };
        [[maybe_unused]] Chromsole chrout{};
    
        enum class ForegroundColor {
                Reset = 0,
                Black = 30,
                Red = 31,
                Green = 32,
                Yellow = 33,
                Blue = 34,
                Magenta = 35,
                Cyan = 36,
                White = 37,
                
                BrightBlack = 90,
                BrightRed = 91,
                BrightGreen = 92,
                BrightYellow = 93,
                BrightBlue = 94,
                BrightMagenta = 95,
                BrightCyan = 96,
                BrightWhite = 97
        };
        
        enum class BackgroundColor {
                Reset = 0,
                Black = 40,
                Red = 41,
                Green = 42,
                Yellow = 43,
                Blue = 44,
                Magenta = 45,
                Cyan = 46,
                White = 47,
                
                BrightBlack = 100,
                BrightRed = 101,
                BrightGreen = 102,
                BrightYellow = 103,
                BrightBlue = 104,
                BrightMagenta = 105,
                BrightCyan = 106,
                BrightWhite = 107
        };
    
        std::unordered_map<std::string, std::variant<ForegroundColor, BackgroundColor>> colorMap = {
                {"R", ForegroundColor::Reset},
                
                {"Fblk", ForegroundColor::Black},
                {"Fred", ForegroundColor::Red},
                {"Fgrn", ForegroundColor::Green},
                {"Fylw", ForegroundColor::Yellow},
                {"Fblu", ForegroundColor::Blue},
                {"Fmag", ForegroundColor::Magenta},
                {"Fcyn", ForegroundColor::Cyan},
                {"Fwht", ForegroundColor::White},
                {"FBblk", ForegroundColor::BrightBlack},
                {"FBred", ForegroundColor::BrightRed},
                {"FBgrn", ForegroundColor::BrightGreen},
                {"FBylw", ForegroundColor::BrightYellow},
                {"FBblu", ForegroundColor::BrightBlue},
                {"FBmag", ForegroundColor::BrightMagenta},
                {"FBcyn", ForegroundColor::BrightCyan},
                {"FBwht", ForegroundColor::BrightWhite},
            
                {"Bblk", BackgroundColor::Black},
                {"Bred", BackgroundColor::Red},
                {"Bgrn", BackgroundColor::Green},
                {"Bylw", BackgroundColor::Yellow},
                {"Bblu", BackgroundColor::Blue},
                {"Bmag", BackgroundColor::Magenta},
                {"Bcyn", BackgroundColor::Cyan},
                {"Bwht", BackgroundColor::White},
                {"BBblk", BackgroundColor::BrightBlack},
                {"BBred", BackgroundColor::BrightRed},
                {"BBgrn", BackgroundColor::BrightGreen},
                {"BBylw", BackgroundColor::BrightYellow},
                {"BBblu", BackgroundColor::BrightBlue},
                {"BBmag", BackgroundColor::BrightMagenta},
                {"BBcyn", BackgroundColor::BrightCyan},
                {"BBwht", BackgroundColor::BrightWhite}
        };
}