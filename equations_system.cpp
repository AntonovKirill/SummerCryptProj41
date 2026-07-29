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

// вспомогательная структура для разреженного уравнения
struct SparseEquation // то есть вместо [1, 0, 0, 1, 0] -> [0, 3]
{
    std::vector<int> indices; // индексы столбцов, где стоят 1 ([1, 0, 0, 1, 0] -> [0, 3])
    uint8_t free_term{0};     // свободный член (0 или 1)
    bool is_empty() const     // остались ли в уравнении переменные, если вектор пуст, то 0 = free_term (несовместное)
    {
        return indices.empty();
    }
    int leading_var() const // индекс самой первой переменной в уравнении
    {
        return indices.empty() ? -1 : indices.front();
    }
};

// операция XOR для двух разреженных строк
static void xor_sparse_rows(SparseEquation &target, const SparseEquation &source) // строку target обнуляем, source -- опорная стрка
{
    std::vector<int> result;
    result.reserve(target.indices.size() + source.indices.size());
    size_t i = 0, j = 0;
    while (i < target.indices.size() && j < source.indices.size())
    {
        // если элемент только в target или только в source, сохраняем в result
        if (target.indices[i] < source.indices[j])
        {
            result.push_back(target.indices[i++]);
        }
        else if (target.indices[i] > source.indices[j])
        {
            result.push_back(source.indices[j++]);
        }
        else // если переменная в обеих строках, она сокращается (сдвигаем указатели, не добавляя индекс в result)
        {
            // 1 XOR 1 = 0 (элементы сокращаются)
            ++i;
            ++j;
        }
    }
    // дописываем оставгиеся элементы из длинного массива
    while (i < target.indices.size())
        result.push_back(target.indices[i++]);
    while (j < source.indices.size())
        result.push_back(source.indices[j++]);

    target.indices = std::move(result);   // перезаписываем target
    target.free_term ^= source.free_term; // xor свободных членов
}

bool MQS_system::solve_gauss()
{
    std::vector<SparseEquation> sparse_rows; // массив для хранения разреженных уравнений
    sparse_rows.reserve(equations.size());
    for (const auto &eq : equations) // перебираем каждое исходное уравнение
    {
        SparseEquation sp_eq;
        sp_eq.free_term = 0;
        const auto &literals = eq.get_param_form();
        for (const auto &lit : literals)
        {
            int raw_id = lit.get_id(); // id литерала (нч == нот(xi) == xi xor 1 -- 1 уносим в free_term)
            int var_id = raw_id / 2;   //  x_i имеет индекс i = raw_id / 2
            sp_eq.indices.push_back(var_id);
            if (raw_id % 2 != 0) // (нч == нот(xi) == xi xor 1 -- 1 уносим в free_term)
            {
                sp_eq.free_term ^= 1;
            }
        }

        std::sort(sp_eq.indices.begin(), sp_eq.indices.end()); // сортировка для метода двух указателей
        std::vector<int> unique_indices;
        unique_indices.reserve(sp_eq.indices.size());
        // убираем дубликаты (так как x_i XOR x_i = 0)
        for (int idx : sp_eq.indices)
        {
            if (!unique_indices.empty() && unique_indices.back() == idx)
            {
                unique_indices.pop_back(); // 1 xor 1 = 0 (переменная сократилась)
            }
            else
            {
                unique_indices.push_back(idx);
            }
        }
        sp_eq.indices = std::move(unique_indices);
        if (!sp_eq.is_empty() || sp_eq.free_term != 0) // не 0 == 0
        {
            sparse_rows.push_back(std::move(sp_eq));
        }
    }
    // прямой ход алгоритма Гаусса
    size_t total_vars = params.size();
    std::vector<int> pivot_row(total_vars, -1);
    // pivot_row[k] хранит индекс строки, к-ая явл опорной для переменной xk
    for (size_t i = 0; i < sparse_rows.size(); ++i)
    {
        while (!sparse_rows[i].is_empty())
        {
            int lead = sparse_rows[i].leading_var();                 // ведущая переменная
            if (lead < 0 || static_cast<size_t>(lead) >= total_vars) // проверка выхода за границы
            {
                break;
            }
            if (pivot_row[lead] != -1) // если уже есть опорная строка (xor текущей с этой, lead зануляется, while проверяет след переменную)
            {
                int p_idx = pivot_row[lead];
                xor_sparse_rows(sparse_rows[i], sparse_rows[p_idx]);
            }
            else
            {
                pivot_row[lead] = static_cast<int>(i); // новая опорная строка
                break;
            }
        }
        // проверка на противоречие (совместность)
        if (sparse_rows[i].is_empty() && sparse_rows[i].free_term != 0)
        {
            return true; // система несовместна (противоречие)
        }
    }
    return false; // система совместна
}

MQS_system MQS_system::create(std::string filepath)
{
    //В разработке
    return NULL;
}
