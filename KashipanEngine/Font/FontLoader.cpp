#include <fstream>
#include <sstream>
#include "FontLoader.h"

namespace KashipanEngine {

namespace {

/// @brief 文字の両辺にあるダブルクォーテーションを消す
/// @param input ダブルクォーテーションを消したい文字列
/// @return ダブルクォーテーションを消した後の文字列
std::string StripQuotes(const std::string &input) {
    if (input.size() >= 2 && input.front() == '"' && input.back() == '"') {
        return input.substr(1, input.size() - 2);
    }
    return input;  // クォートがなければそのまま返す
}

FontInfo LoadFontInfo(std::istringstream &iss) {
    FontInfo fontInfo{};

    // 行を空白ごとに読み込み
    std::string param;
    while (iss >> param) {
        // 区切った文字を "=" で区切る
        size_t pos = param.find('=');
        std::string key;
        std::string value;
        if (pos == std::string::npos) {
            continue; // '='が見つからないものは無視
        } else {
            key = param.substr(0, pos);
            value = param.substr(pos + 1);
        }

        if (key == "face") {
            //--------- face ---------//
            fontInfo.face = StripQuotes(value);

        } else if (key == "size") {
            //--------- size ---------//
            fontInfo.size = std::stoi(value);

        } else if (key == "bold") {
            //--------- bold ---------//
            int flag = std::stoi(value);
            if (flag == 1) {
                fontInfo.isBold = true;
            } else {
                fontInfo.isBold = false;
            }

        } else if (key == "italic") {
            //--------- italic ---------//
            int flag = std::stoi(value);
            if (flag == 1) {
                fontInfo.isItalic = true;
            } else {
                fontInfo.isItalic = false;
            }

        } else if (key == "charset") {
            //--------- charset ---------//
            fontInfo.charset = StripQuotes(value);

        } else if (key == "unicode") {
            //--------- unicode ---------//
            int flag = std::stoi(value);
            if (flag == 1) {
                fontInfo.isUnicode = true;
            } else {
                fontInfo.isUnicode = false;
            }

        } else if (key == "stretchH") {
            //--------- stretchH ---------//
            fontInfo.stretchH = (std::stof(value)) / 100.0f;

        } else if (key == "smooth") {
            //--------- smooth ---------//
            int flag = std::stoi(value);
            if (flag == 1) {
                fontInfo.isSmooth = true;
            } else {
                fontInfo.isSmooth = false;
            }

        } else if (key == "aa") {
            //--------- aa ---------//
            fontInfo.aa = std::stoi(value);

        } else if (key == "padding") {
            //--------- padding ---------//
            int i = 0;
            std::istringstream paddingStream(value);
            std::string paddingValue;
            while (std::getline(paddingStream, paddingValue, ',')) {
                if (i < 4) {
                    fontInfo.padding[i] = std::stoi(paddingValue);
                }
                i++;
            }

        } else if (key == "spacing") {
            //--------- spacing ---------//
            int i = 0;
            std::istringstream spacingStream(value);
            std::string spacingValue;
            while (std::getline(spacingStream, spacingValue, ',')) {
                if (i < 2) {
                    fontInfo.spacing[i] = std::stoi(spacingValue);
                }
                i++;
            }

        } else if (key == "outline") {
            //--------- outline ---------//
            fontInfo.outline = std::stoi(value);
        }
    }

    return fontInfo;
}

FontCommon LoadFontCommon (std::istringstream &iss) {
    FontCommon fontCommon{};

    // 行を空白ごとに読み込み
    std::string param;
    while (iss >> param) {
        // 区切った文字を "=" で区切る
        size_t pos = param.find('=');
        std::string key;
        std::string value;
        if (pos == std::string::npos) {
            continue; // '='が見つからないものは無視
        } else {
            key = param.substr(0, pos);
            value = param.substr(pos + 1);
        }

        if (key == "lineHeight") {
            //--------- lineHeight ---------//
            fontCommon.lineHeight = std::stof(value);

        } else if (key == "base") {
            //--------- base ---------//
            fontCommon.base = std::stof(value);

        } else if (key == "scaleW") {
            //--------- scaleW ---------//
            fontCommon.scaleW = std::stof(value);

        } else if (key == "scaleH") {
            //--------- scaleH ---------//
            fontCommon.scaleH = std::stof(value);

        } else if (key == "pages") {
            //--------- pages ---------//
            fontCommon.pages = std::stoi(value);

        } else if (key == "packed") {
            //--------- packed ---------//
            int flag = std::stoi(value);
            if (flag == 1) {
                fontCommon.isPacked = true;
            } else {
                fontCommon.isPacked = false;
            }

        } else if (key == "alphaChnl") {
            //--------- alphaChnl ---------//
            fontCommon.alphaChannel = std::stoi(value);

        } else if (key == "redChnl") {
            //--------- redChnl ---------//
            fontCommon.redChannel = std::stoi(value);

        } else if (key == "greenChnl") {
            //--------- greenChnl ---------//
            fontCommon.greenChannel = std::stoi(value);

        } else if (key == "blueChnl") {
            //--------- blueChnl ---------//
            fontCommon.blueChannel = std::stoi(value);
        }
    }

    return fontCommon;
}

FontPage LoadFontPage(std::istringstream &iss) {
    FontPage fontPage{};

    // 行を空白ごとに読み込み
    std::string param;
    while (iss >> param) {
        // 区切った文字を "=" で区切る
        size_t pos = param.find('=');
        std::string key;
        std::string value;
        if (pos == std::string::npos) {
            continue; // '='が見つからないものは無視
        } else {
            key = param.substr(0, pos);
            value = param.substr(pos + 1);
        }

        if (key == "id") {
            //--------- id ---------//
            fontPage.id = std::stoi(value);
        
        } else if (key == "file") {
            //--------- file ---------//
            fontPage.file = StripQuotes(value);
        }
    }

    return fontPage;
}

CharInfo LoadCharInfo(std::istringstream &iss) {
    CharInfo charInfo{};

    // 行を空白ごとに読み込み
    std::string param;
    while (iss >> param) {
        // 区切った文字を "=" で区切る
        size_t pos = param.find('=');
        std::string key;
        std::string value;
        if (pos == std::string::npos) {
            continue; // '='が見つからないものは無視
        } else {
            key = param.substr(0, pos);
            value = param.substr(pos + 1);
        }

        if (key == "id") {
            //--------- id ---------//
            charInfo.id = std::stoi(value);
        
        } else if (key == "x") {
            //--------- x ---------//
            charInfo.x = std::stof(value);
        
        } else if (key == "y") {
            //--------- y ---------//
            charInfo.y = std::stof(value);
        
        } else if (key == "width") {
            //--------- width ---------//
            charInfo.width = std::stof(value);
        
        } else if (key == "height") {
            //--------- height ---------//
            charInfo.height = std::stof(value);
        
        } else if (key == "xoffset") {
            //--------- xoffset ---------//
            charInfo.xOffset = std::stof(value);
        
        } else if (key == "yoffset") {
            //--------- yoffset ---------//
            charInfo.yOffset = std::stof(value);
        
        } else if (key == "xadvance") {
            //--------- xadvance ---------//
            charInfo.xAdvance = std::stof(value);
        
        } else if (key == "page") {
            //--------- page ---------//
            charInfo.page = std::stoi(value);
        
        } else if (key == "chnl") {
            //--------- chnl ---------//
            charInfo.channel = std::stoi(value);
        }
    }

    return charInfo;
}

} // namespace

FontData LoadFNT(const char *fntFilePath) {
    FontData fontData;
    std::ifstream file(fntFilePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open font file: " + std::string(fntFilePath));
    }

    // 行ごとの読み込み
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string identifier;
        iss >> identifier;
        
        // 行の先頭の文字ごとに処理切り替え
        if (identifier == "char") {
            CharInfo charInfo = LoadCharInfo(iss);
            fontData.chars.emplace(charInfo.id, charInfo);

        } else if (identifier == "chars") {
            std::string chars;
            iss >> chars;
            size_t pos = chars.find('=');
            if (pos != std::string::npos) {
                fontData.charsCount = std::stoi(chars.substr(pos + 1));
            }

        } else if (identifier == "page") {
            FontPage fontPage = LoadFontPage(iss);
            fontData.pages.push_back(fontPage);

        } else if (identifier == "common") {
            fontData.common = LoadFontCommon(iss);
        
        } else if (identifier == "info") {
            fontData.info = LoadFontInfo(iss);
        }
    }

    file.close();
    return fontData;
}

} // namespace KashipanEngine