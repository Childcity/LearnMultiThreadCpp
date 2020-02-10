#include <iostream>
#include <random>
#include <algorithm>

std::string random_string(int max_length)
{
    std::string possible_characters("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_int_distribution<> dist(0, static_cast<int>(possible_characters.size() - 1));
    std::string ret;

    for(int i = 0; i < max_length; i++){
        int random_index = dist(engine); //get index between 0 and possible_characters.size()-1
        ret += possible_characters[random_index];
    }

    return ret;
}

class StringWrapper {
private:
    using string = std::string;

    string str_;

public: /*constructors*/

    // ctor
    explicit StringWrapper(std::string str) noexcept
        : str_(move(str))
    {
        std::cout << "old: " << str_.substr(0, 20) << std::endl;
    }

    // copy
    StringWrapper(const StringWrapper &other)
    {
        if(&other != this){
            str_ = other.str_;
        }
    }

    // move
    StringWrapper(StringWrapper &&other) noexcept
        : str_(move(other.str_))
    {
        other.str_.clear();
    }

    void swap(StringWrapper &other) noexcept
    {
        std::swap(str_, other.str_);
    }

    // swap trick
//    StringWrapper& operator=(const StringWrapper &other){
//        StringWrapper tmp(other);
//        swap(tmp);
//        return *this;
//    }

    // extended swap trick gives
    // copy and move 'operator=' at the same time
    StringWrapper& operator=(StringWrapper other) noexcept {
        swap(other);
        return *this;
    }

    string getStr()
    {
        return str_;
    }

};


int main() {

    StringWrapper second = StringWrapper("second + " + random_string(1000*1000*5));
    StringWrapper first("first + " + random_string(1000*1000*5));

    first = second;             // will be copy ctor
    first = std::move(second);  // will be move ctor

    std::cout << first.getStr().substr(0, 20) << std::endl;
    return 0;
}

