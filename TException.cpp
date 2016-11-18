//
// Created by sm4ll_3gg on 12.11.16.
//

#include "TException.h"

#include <iostream>

TException::TException()
{
    name = "All Right!";
}

TException::TException(const std::string& _name)
{
    name = _name;
    if(name == "Некорректно закодированно дерево!")
    {
        std::cout << name << std::endl;
        std::terminate();
    }
}

TException::TException(const std::string& _name, std::exception& exp)
        :std::exception(exp)
{
    name = _name;
}

std::string TException::get_error_name()
{
    return name;
}