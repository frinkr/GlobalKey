#pragma once
#include <string>

class GKArgs {
public:
    GKArgs(const std::string &args);

    std::string

    head() const;

    std::string
    take_head();

    
    std::string
    rest() const;

    const std::string &
    all() const;
    
private:
    std::size_t
    advance() const;

    std::size_t
    skip_spaces(std::size_t pos) const;
    
private:
    std::string args_ {};
    std::size_t pos_ {};
};
