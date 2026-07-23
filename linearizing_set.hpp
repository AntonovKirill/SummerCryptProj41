#ifndef LINEARIZING_SET
#define LINEARIZING_SET

#include <vector>
#include "equations_system.hpp"

class linearizing_set
{
private:
    std::vector<literal> literals_set; // это и есть линеаризующее множество В = {x1, x2, ... , xm}
public:
    // 2 способа создания на выбор
    linearizing_set();
    linearizing_set(const std::vector<literal> &lits);
    void add_literal(const literal &lit); // добавление одного нового литерала
    void process_tasks(MQS_system &mqs);  // принимает ссылку на сла, генерирует 2^m комбинаций битов, делает подстановку и упрощает
};

#endif
