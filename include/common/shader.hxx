#pragma once

#include "../glad/glad.h"
#include<iostream>
#include<fstream>
#include<string>

class Shader {
    public:
        unsigned int ID;

        // construct shader from .vs and .fs files
        Shader(const std::string vertexPath, const std::string fragmentPath);

        void use();
};