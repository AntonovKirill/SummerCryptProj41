#ifndef EQUATIONS_SYSTEM
#define EQUATIONS_SYSTEM

#include <iostream>
#include <vector>
#include <bool>
#include <string>
#include <utility>

class literal {
    private:
        std::pair<bool, bool> lit;

        literal();
        literal(bool x);

        ~literal();

    publick:
        bool is_known();

        void set_x(int x);
        void set_neg_x(int neg_x);

        bool get_x() const;
        bool get_neg_x() const;
};

class equation {
    private:
        
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
