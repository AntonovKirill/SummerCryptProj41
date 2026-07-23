#ifndef EQUATIONS_SYSTEM
#define EQUATIONS_SYSTEM

#include <iostream>
#include <vector>
#include <bool>
#include <string>
#include <utility>

class literal {
    private:
        int id;
        char value;

        literal();
        literal(int id, char value);

        ~literal();

    public:
        bool is_known();
        
        void set_value(char value);

        char get_id() const;
        char get_value() const;
};

class equation {
    private:
        bool Is_line;
        int param_num;
        std::vector<literal>;

        equation();
        equation(param_num);
        equation(IS_line, param_num);
        ~equation();


    public:
        is_linear();
};

class MQS_system {
private:
    unsigned int param_num;
    std::vector<equation> equations;

    MQS_system();
    MQS_system(std::vector<equation> equations);

    ~MQS_system();

public:
    MQS_system* create(std::string filename);
    MQS_system* create(std::vector<equation> equations);


    void destroy(MQS_system* instance);

    MQS_system unit_propagation();
    void add_equation(equation eq);
    bool is_linear();
};

#endif
