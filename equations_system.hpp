#ifndef EQUATIONS_SYSTEM
#define EQUATIONS_SYSTEM

#include <iostream>
#include <vector>

class literal {
};

class MQS_system {
private:
    unsigned int maximum_variable_index;
    unsigned int number_of_inputs;
    unsigned int number_of_latches = 0;
    unsigned int number_of_outputs;
    unsigned int number_of_AND_gates;
    std::vector(unsigned int) inputs;
    std::vector(unsigned int) outputs;
    std::vector(unsigned int[3]) AND_gates;

    void set_maximum_variable_index(int value);
    void set_number_of_inputs(int value);
    void set_number_of_outputs(int value);
    void set_number_of_AND_gates(int value);

    int get_maximum_variable_index() const;
    int get_number_of_inputs() const;
    int get_number_of_outputs() const;
    int get_number_of_AND_gates() const;

    MQS_encoding();
    MQS_encoding(int max_var_idx, int num_outputs);
    MQS_encoding(int max_var_idx, int num_outputs, int num_inputs);
    MQS_encoding(int max_var_idx, int num_outputs, int num_inputs, int num_and_gates);

    ~MQS_encoding();

public:
    MQS_encoding* create();
    MQS_encoding* create(int max_var_idx, int num_outputs);
    MQS_encoding* create(int max_var_idx, int num_outputs, int num_inputs);
    MQS_encoding* create(int max_var_idx, int num_outputs, int num_inputs, int num_and_gates);

    void destroy(MQS_encoding* instance);

};

#endif
