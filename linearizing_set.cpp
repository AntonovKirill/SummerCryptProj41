#include "linearizing_set.hpp"
#include <iostream>

linearizing_set::linearizing_set()
{
}

linearizing_set::linearizing_set(const std::vector<literal> &lits)
    : literals_set(lits)
{
}

void linearizing_set::add_literal(const literal &lit) // метод добавления одного литерала
{
    literals_set.push_back(lit);
}

void linearizing_set::process_tasks(MQS_system &mqs) // перебор 2^m подзадач
{
    size_t m = literals_set.size();
    if (m == 0) // если множество B пустое
    {
        std::cout << "Линеаризующее множество пустое!\n";
        return;
    }
    size_t total_tasks = 1ULL << m; // так 2 в степени m пишется
    std::cout << "Всего подзадач для перебора: " << total_tasks << "\n";
    for (size_t task_index = 0; task_index < total_tasks; ++task_index)
    {
        MQS_system current_mqs = mqs; // делаем копию исходной системы для текущей подзадачи
        bool conflict = false;
        for (size_t i = 0; i < m; ++i)
        {
            bool bit_val = (task_index >> i) & 1; // считываем iый бит числа task_index
            char val_char;
            if (bit_val == true)
            {
                val_char = '1';
            }
            else
            {
                val_char = '0';
            }
            literals_set[i].set_value(val_char);               // сохраняет символьное значение внутри iого литерала
            if (current_mqs.unit_propagation(literals_set[i])) // подставляет зафиксированный литерал в текущую систему
            {
                conflict = true;
                break; // ветвь несовместна, дальше подставлять биты нет смысла
            }
        }
        if (conflict)
        {
            continue;
        }
        bool is_inconsistent = current_mqs.solve_gauss(); // решаем получившуюся линейную систему методом Гаусса над F_2
        if (!is_inconsistent)                             // 0 = система совместна
        {
            std::cout << "Подзадача " << task_index << " успешно решена!\n";
        }
    }
}
