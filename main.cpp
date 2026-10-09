#include "test_runner.h"
#include <fstream>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <sstream>
#include <stack>
#include <string>

void PrintJsonString(std::ostream& out, std::string_view str) {
    for(const char c : str){
        if(c == '\\'){
            out << '\\' << '\\';
        } else if(c == '"'){
            out << '\\' << '"';
        } else {
            out << c;
        }
    }
}

class EmptyContext {
public:
    explicit EmptyContext(std::ostream& ss) : ss_(ss)
    {}
private:
    std::ostream& ss_;
};

template <typename T>
class  ArrayContext{
public:
    ArrayContext(std::ostream& ss, T& context) : ss_(ss), context_(context), need_comma_(true)
    {}

    ArrayContext& BeginArray(){
        ss_ << '[';
//        Context context_child = *this;
//        ArrayContext ac(ss_, context_child);
//        return ac;
        throw std::runtime_error("BeginArray");
    }

    T& EndArray(){
        ss_ << ']';
        return context_;
    }

    ArrayContext Number(int64_t number){
        if(need_comma_){
            ss_ << ',';
            need_comma_ = false;
        }
        ss_ << number;
        return *this;
    }
    ArrayContext String(std::string_view s){
        if(need_comma_){
            ss_ << ',';
            need_comma_ = false;
        }
        PrintJsonString(ss_, s);
        return *this;
    }
private:
    std::ostream& ss_;
    bool need_comma_;
    T& context_;
};



//using ArrayContext = ArrayPrint<EmptyContext>;  // Замените это объявление на определение типа ArrayContext
ArrayContext PrintJsonArray(std::ostream& out) {
    EmptyContext ec(out);
    ArrayContext ac(out, ec);
    return ac;
}

//using ObjectContext = void;  // Замените это объявление на определение типа ObjectContext
struct  ObjectContext{};
ObjectContext PrintJsonObject(std::ostream& out) {
    // реализуйте функцию
    throw std::runtime_error("PrintJsonObject");
}

void TestArray() {
    std::ostringstream output;

    {
        auto json = PrintJsonArray(output);
        json
                .Number(5)
                .Number(6)
                .BeginArray()
                .Number(7)
                .EndArray()
                .Number(8)
                .String("bingo!");
    }

    ASSERT_EQUAL(output.str(), R"([5,6,[7],8,"bingo!"])");
}

//void TestObject() {
//    std::ostringstream output;
//
//    {
//        auto json = PrintJsonObject(output);
//        json
//                .Key("id1").Number(1234)
//                .Key("id2").Boolean(false)
//                .Key("").Null()
//                .Key("\"").String("\\");
//    }
//
//    ASSERT_EQUAL(output.str(), R"({"id1":1234,"id2":false,"":null,"\"":"\\"})");
//}

void TestAutoClose() {
    std::ostringstream output;

    {
        auto json = PrintJsonArray(output);
        //json.BeginArray().BeginObject();
        json.BeginArray();
    }

    //ASSERT_EQUAL(output.str(), R"([[{}]])");
    ASSERT_EQUAL(output.str(), R"([[]])");
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestArray);
    //RUN_TEST(tr, TestObject);
    RUN_TEST(tr, TestAutoClose);

    return 0;
}
