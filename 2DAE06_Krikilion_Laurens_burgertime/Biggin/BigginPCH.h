#pragma once
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include <stdio.h>
#include <iostream> // std::cout
#include <sstream> // stringstream
#include <memory> // smart pointers
#include <vector>


//Allows usage of std::min and max
#define NOMINMAX

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma warning(push, 0)
#include <glm/glm.hpp>
#include <SDL.h>
#pragma warning (pop)
