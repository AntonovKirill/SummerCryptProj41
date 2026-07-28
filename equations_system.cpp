#include "equations_system.hpp"

literal::literal() : id(-1), value(2) {}

literal::literal(int id) : id(id0), value(2) {}

literal::literal(int id, std::uint8_t value0) : id(id0), value(value0) {}

literal::~literal() {}

void literal::set_value(std::uint8_t value0)
{
    value = value0;
}

std::uint8_t literal::get_value() const
{
    return value;
}

/*
literal* literal::create()
{
    literal* ret = new literal;
    return ret;
}

literal* literal::create(int id0)
{
    literal* ret = new literal(id0);
    return ret;
}

literal* literal::create(int id0, std::uint8_t value0)
{
    literal* ret = new literal(id0, value0);
    return ret;
}
*/

void literal::destroy()
{
    delete this;
}

bool literal::is_known()
{
    return (value <= 1);
}

equation::equation() : Is_line(0), ids() {}

equation::equation(Is_line0) : Is_line(Is_line0), ids() {}

equation::equation(ids0) : Is_line(0), ids(ids0) {}

equation::equation(Is_line0, ids0) : Is_line(Is_line0), ids(ids0) {}

equation::~equation() {}
/*
equation* equation::create(Is_line0, param_form0)
{
    equation* ret = new equation(Is_line0, param_form0);
    return ret;
}
*/
void equation::destroy()
{
    delete this;
}

void equation::set_Is_line(bool Is_line0)
{
    Is_line = Is_line0;
}

void equation::set_ids(std::vector<int> ids0)
{
    ids = ids0;
}

bool equation::get_Is_line() const
{
    return Is_line;
}

std::vector<int> equation::get_ids() const
{
    return ids;
}

bool equation::is_linear()
{
    return (Is_line == 1);
}

MQS_system::MQS_system() : params(), equations() {}

MQS_system::MQS_system(std::vector<equation> equations0) : params(), equations(equations0)
{
    for (int i = 0; i < equations.size(); i++)
    {
        for (int j = 0; j < equations[i].ids.size(); j++)
        {
            if ( (equations[i].ids[j] / 2 * 2 + 1) > params.size() )
            {
                params.resize( (equations[i].ids[j] / 2 * 2 + 2), literal() );
            }
        }
    }
}

MQS_system::~MQS_system
{
    params.clear();
    equations.clear();
}

void MQS_system::destroy()
{
    delete this;
}

void set_params(std::vector<literal> params0)
{
    params = params0;
}

void set_equations(std::vector<equation> equations0)
{
    equations = equations0;
}

std::vector<literal*> get_params() const
{
    return params;
}

std::vector<equation> get_equations() const
{
    return equations;
}

MQS_system MQS_system::unit_propagation()
{
    std::vector<equation> equations0;
    int c = 0;
    for (int i=0; i<equations.size(); i++)
    {
        c = 0;
        for (int j = 0; j < equations[i].ids.size(); j++)
        {
            if(params[equations[i].ids[j]].is_known == 0)
            {
                c++;
            }
        }
        if (c>=2)
        {
            equations0.push_back();
        }
    }
    MQS_system ret(equations0);
    retrun ret;
}

void MQS_system::add_equation(equation eq)
{
    equations.push_back(eq);
    for (int i=0; i < eq.ids.size(); i++)
    {
        if (eq.ids[i] >= params.size())
        {
            params.resize(eq.ids[i] / 2 * 2 + 2);
        }
    }
}

bool MQS_system::is_linear()
{
    for (int i=0; i < equations.size(); i++)
    {
        if (equations[i].is_linear() == 0)
        {
            return 0;
        }
    }
    return 1;
}

bool MQS_system::add_literals_value()
{
    for (i = 0; i < params.size(); i = i + 2)
    {
        if ( (params[i].is_known() == 1) and (params[i+1].is_known() == 1) and ((params[i+1].value ^ params[i].value) == 0) )
        {
            return 1;
        }
        if ( (params[i].is_known()) ^ (params[i+1].is_known()) )
        {
            if (params[i].is_known() == 1)
            {
                params[i+1].value = (params[i].value ^ 1);
            }
            else
            {
                params[i].value = (params[i+1].value ^ 1);
            }
        }
    }
    return 0;
}

bool add_literal_value(literal new_lit)
{
    if (params[new_lit.id].is_known == 1 and params[new_lit.id].value != new_lit.value)
    {
        return 1;
    }
    if ( (new_lit.id % 2 == 1) and (params[new_lit.id - 1].is_known) and (new_lit.value == params[new_lit.id - 1].value) )
    {
        return 1;
    }
    if ( (new_lit.id % 2 == 0) and (params[new_lit.id + 1].is_known) and (new_lit.value == params[new_lit.id + 1].value) )
    {
        return 1;
    }

    params[new_lit.id] == new_lit;

    return 0;
}

bool MQS_system::solve_gauss()
{
    if (this->is_linear() != 0) // проверка линейности системы
    {
        return 1;
    }
    size_t n_eqs = equations.size(); // кол-во уравнений в с-ме
    if (n_eqs == 0)
        return 0;

    size_t n_vars = param_num.size(); // кол-во переменных в с-ме (размерность)
    std::vector<std::vector<uint8_t>> matrix(n_eqs, std::vector<uint8_t>(n_vars + 1, 0));
    // двумерный вектор размером n_eqs строк на n_vars + 1 столбцов (+1 для свободных членов)
    for (size_t i = 0; i < n_eqs; ++i)
    {
        std::vector<literal *> lits = equations[i].get_ids(); // берем список его литералов
        for (size_t j = 0; j < lits.size(); ++j)
        {
            literal *lit = lits[j];
            int var_index = lit->get_id() / 2; // вычисляем индекс переменной
            if (var_index < n_vars)
            {
                matrix[i][var_index] ^= 1; // ставим 1 в соотв ячейку матрицы (xor чтобы учесть возможное сложение одинаковых переменных)
            }
        }
        matrix[i][n_vars] = equations[i].get_free_term(); // свободный член заполняем
    }

    size_t pivot_row = 0; // индекс опорной строки (в какую строку мы записываем текущий главный элемент)
    for (size_t col = 0; col < n_vars && pivot_row < n_eqs; ++col)
    {
        size_t sel = pivot_row;
        while (sel < n_eqs && matrix[sel][col] == 0)
        {
            ++sel;
        }
        if (sel == n_eqs)
        {
            continue;
        }
        std::swap(matrix[pivot_row], matrix[sel]); // перестановка строк
        for (size_t r = 0; r < n_eqs; ++r)         // обнуление столбца
        {
            if (r != pivot_row && matrix[r][col] == 1)
            {
                for (size_t c = col; c <= n_vars; ++c)
                {
                    matrix[r][c] ^= matrix[pivot_row][c];
                }
            }
        }
        ++pivot_row;
    }

    for (size_t r = 0; r < n_eqs; ++r) // проверка на совместность системы (нет ли 00000 | 1)
    {
        bool all_zeros = true;
        for (size_t c = 0; c < n_vars; ++c)
        {
            if (matrix[r][c] == 1)
            {
                all_zeros = false;
                break;
            }
        }
        if (all_zeros && matrix[r][n_vars] == 1)
        {
            return 1;
        }
    }

    return 0;
}

MQS_system MQS_system::create(std::string filepath)
{
    //В разработке
    return NULL;
}
