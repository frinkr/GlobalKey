#include <cassert>
#include "GKArgs.h"

namespace {
    template <typename T>
    bool is_space(T && v) {
        return v == ' ';
    }

    constexpr char quote1 = '\'';
    constexpr char quote2 = '"';
}

GKArgs::GKArgs(const std::string &args)
    : args_(args) {
    pos_ = skip_spaces(0);
}

std::string
GKArgs::head() const {
    if (auto pos = advance(); pos != std::string::npos) 
        return args_.substr(pos_, pos - pos_);
    else
        return {};
}

std::string
GKArgs::take_head() {
    if (auto h = head(); !h.empty()) {
        pos_ = skip_spaces(pos_ + h.size());
        return h;
    }
    else {
        return {};
    }
}

bool
GKArgs::done() const {
    return pos_ >= args_.size();
}

std::string
GKArgs::rest() const {
    return args_.substr(pos_);
}

const std::string &
GKArgs::all() const {
    return args_;
}

std::size_t
GKArgs::advance() const {
    if (pos_ >= args_.size())
        return std::string::npos;

    assert(!is_space(args_[pos_]));
    
    if (args_[pos_] == quote1 || args_[pos_] == quote2) {
        auto next = args_.find_first_of(args_[pos_], pos_ + 1);
        if (next != std::string::npos) {
            // end of string
            if (next + 1 == args_.size())
                return next + 1;

            // space follows quote
            if (is_space(args_[next + 1]))
                return next + 1;
        }
    }

    // next space
    auto next = args_.find_first_of(' ', pos_);
    if (next == std::string::npos)
        return args_.size();
    else
        return next;
}
    
std::size_t
GKArgs::skip_spaces(std::size_t pos) const {
    while (pos < args_.size() && is_space(args_[pos]))
        ++ pos;
    return pos;
}
