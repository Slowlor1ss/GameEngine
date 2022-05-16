#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

//https://stackoverflow.com/questions/2573834/c-convert-string-or-char-to-wstring-or-wchar-t
#include <locale>
#include <codecvt>
#include <string>

inline std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
//Example
//std::string narrow = converter.to_bytes(wide_utf16_source_string);
//std::wstring wide = converter.from_bytes(narrow_utf8_source_string);

inline std::string TrimLeft(const std::string& str, const std::string& token = " ")
{
    std::string t = str;
    t.erase(0, t.find_first_not_of(token));
    return t;
}

inline std::string TrimRight(const std::string& str, const std::string& token = " ")
{
    std::string t = str;
    t.erase(t.find_last_not_of(token) + 1);
    return t;
}

inline std::string Trim(const std::string& str, const std::string& token = " ")
{
    std::string t = str;
    t.erase(0, t.find_first_not_of(token));
    t.erase(t.find_last_not_of(token) + 1);
    return t;
}


inline std::string ToLower(const std::string& str)
{
    std::string t = str;
    std::transform(t.begin(), t.end(), t.begin(),
        [](const unsigned char c) { return static_cast<const unsigned char>(std::tolower(c)); });
    return t;
}

inline std::string ToUpper(const std::string& str)
{
    std::string t = str;
    std::transform(t.begin(), t.end(), t.begin(),
        [](const unsigned char c) { return static_cast<const unsigned char>(std::toupper(c)); });
    return t;
}

inline bool StartsWith(const std::string& str, const std::string& substr)
{
    return str.find(substr) == 0;
}

inline bool EndsWith(const std::string& str, const std::string& substr)
{
    return str.rfind(substr) == (str.length() - substr.length());
}

//checks if 2 strings are equal without casing about uppercase letters
inline bool EqualsIgnoreCase(const std::string& str1, const std::string& str2)
{
    return ToLower(str1) == ToLower(str2);
}

inline std::vector<std::string> Split(const std::string& str, const std::string& delim, const bool trim_empty = false)
{
    size_t pos, last_pos = 0, len;
    std::vector<std::string> tokens;

    while (true) {
        pos = str.find(delim, last_pos);
        if (pos == std::string::npos) {
            pos = str.size();
        }

        len = pos - last_pos;
        if (!trim_empty || len != 0) {
            tokens.push_back(str.substr(last_pos, len));
        }

        if (pos == str.size()) {
            break;
        }
        else {
            last_pos = pos + delim.size();
        }
    }

    return tokens;
}

inline bool Contains(const std::string& str, const std::string& token)
{
    return str.find(token) == std::string::npos ? false : true;
}

inline std::string Repeat(const std::string& str, unsigned int times)
{
    std::stringstream ss;
    for (unsigned int i = 0; i < times; ++i) {
        ss << str;
    }
    return ss.str();
}

inline std::vector<std::string> Compact(const std::vector<std::string>& tokens)
{
    std::vector<std::string> compacted;

    for (size_t i = 0; i < tokens.size(); ++i) {
        if (!tokens[i].empty()) {
            compacted.push_back(tokens[i]);
        }
    }

    return compacted;
}

inline std::string Join(const std::vector<std::string>& tokens, const std::string& delim, const bool trim_empty = false)
{
    if (trim_empty) {
        return Join(Compact(tokens), delim, false);
    }
    else {
        std::stringstream ss;
        for (size_t i = 0; i < tokens.size() - 1; ++i) {
            ss << tokens[i] << delim;
        }
        ss << tokens[tokens.size() - 1];

        return ss.str();
    }
}

inline std::string ReplacePart(const std::string& source, const std::string& target, const std::string& replacement)
{
    return Join(Split(source, target), replacement, false);
}

inline std::string Replace(const std::string& source, const char* key, const char* with)
{
    std::string new_string;
    size_t search_from = 0;
    size_t result;

    while ((result = source.find(key, search_from)) != std::string::npos) {
        new_string += source.substr(search_from, result - search_from);
        new_string += with;
        size_t k = 0;
        while (key[k] != '\0') {
            k++;
        }
        search_from = result + k;
    }

    if (search_from == 0) {
        return source;
    }

    new_string += source.substr(search_from, source.length() - search_from);

    return new_string;
}


///////////////////////////////

inline std::string ReadFile(const std::string& filepath)
{
    std::ifstream ifs(filepath.c_str());
    if (!ifs.is_open())
    {
#ifdef _DEBUG
        std::cout << "failed to open " << filepath;
#endif
        return {};
    }
    std::string content((std::istreambuf_iterator<char>(ifs)),
        (std::istreambuf_iterator<char>()));
    ifs.close();
    return content;
}

inline void WriteFile(const std::string& filepath, const std::string& content)
{
    std::ofstream ofs(filepath.c_str());

    if (!ofs.is_open())
    {
#ifdef _DEBUG
        std::cout << "failed to open " << filepath;
#endif
        return;
    }

    ofs << content;
    ofs.close();
}