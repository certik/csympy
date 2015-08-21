#ifndef SYMENGINE_VALUE_H
#define SYMENGINE_VALUE_H

#include <string>
#include <map>

namespace SymEngine { namespace value {
    // All value based classes are in the SymEngine::value namespace, so that
    // their names do not collide.

class Basic {
};

class Symbol : public Basic {
private:
    unsigned int name_id_;
    static std::map<std::string, unsigned int> map_name2id_;
public:
    Symbol(const std::string &name) {
        auto it = map_name2id_.find(name);
        if (it == map_name2id_.end()) {
            name_id_ = map_name2id_.size();
            map_name2id_[name] = name_id_;
        } else {
            name_id_ = it->second;
        }
    }

    Symbol(const Symbol &s) = default;
    Symbol(Symbol &&s) = default;
    Symbol &operator=(const Symbol &) = default;
    Symbol &operator=(Symbol &&other) = default;
    ~Symbol() = default;
};

static_assert(sizeof(Symbol) == sizeof(unsigned int), "Symbol size is wrong");
//static_assert(std::is_trivially_copiable<Symbol>::value, "Symbol type is wrong");


} // value
} // SymEngine

#endif
