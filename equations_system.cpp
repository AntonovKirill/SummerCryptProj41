#include <vector>
#include <set>
#include <string>
#include <fstream>
#include <sstream>

#include "equations_system.hpp"

literal::literal() : id(-1), value(2) {}

literal::literal(int id0) : id(id0), value(2) {}

literal::literal(int id0, std::uint8_t value0) : id(id0), value(value0) {}

literal::~literal() {}

void literal::set_value(std::uint8_t value0)
{
    value = value0;
}

std::uint8_t literal::get_value() const
{
    return value;
}

int literal::get_id() const
{
    return this->id;
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

bool literal::is_known() const
{
    return (value <= 1);
}

equation::equation() : Is_line(false), ids() {}

equation::equation(bool Is_line0) : Is_line(Is_line0), ids() {}

equation::equation(const std::vector<int> &ids) : Is_line(0), ids(ids) {}

equation::equation(bool Is_line0, const std::vector<int> &ids) : Is_line(Is_line0), ids(ids) {}

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
    return this->ids;
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
        for (int j = 0; j < equations[i].get_ids().size(); j++)
        {
            if ((equations[i].get_ids()[j] | 1) > params.size())
            {
                params.resize((equations[i].get_ids()[j] | 0) + 2);
            }
            params[equations[i].get_ids()[j]] = literal(equations[i].get_ids()[j]);
        }
    }

    for (int i = 0; i < params.size(); i++)
    {
        if (params[i].get_id() == -1)
        {
            // TODO: разобраться, что делает эта строчка
            // literal lit(i, params[i + static_cast<int>(std::pow(-1, i % 2))] ^ 1);
            literal lit(i, params[i ^ 1].get_value() ^ 1);
            params[i] = lit;
        }
    }
}

MQS_system::~MQS_system()
{
    params.clear();
    equations.clear();
}

void MQS_system::destroy()
{
    delete this;
}

void MQS_system::set_params(std::vector<literal> params0)
{
    params = params0;
}

void MQS_system::set_equations(std::vector<equation> equations0)
{
    equations = equations0;
}

std::vector<literal> MQS_system::get_params() const
{
    return params;
}

std::vector<equation> MQS_system::get_equations() const
{
    return equations;
}

bool MQS_system::unit_propagation(literal &x)
{
    if (add_literal_value(x))
    {
        return true; // противоречие
    }
    bool changed = false;
    // цикл до тех пор, пока выводятся новые значения литералов (каскадное распространение)
    do
    {
        changed = false;
        std::vector<equation> new_equations;
        new_equations.reserve(equations.size());
        for (auto &eq : equations)
        {
            auto ids = eq.get_ids();
            // ЛИНЕЙНОЕ УРАВНЕНИЕ (XOR)
            if (eq.get_Is_line())
            {
                std::vector<int> remaining_ids;
                uint8_t accumulated_const = 0;
                for (int raw_id : ids)
                {
                    int var_id = raw_id / 2;
                    uint8_t val = params[var_id].get_value(); // значение переменной
                    if (params[var_id].is_known())
                    {
                        uint8_t lit_val = (raw_id % 2 == 0) ? val : (val ^ 1);
                        accumulated_const ^= lit_val;
                        changed = true; // подставили значение — уравнение меняется
                    }
                    else
                    {
                        remaining_ids.push_back(raw_id); // переменная неизвестна, оставляем литерал
                    }
                }
                // если не осталось переменных в уравнении
                if (remaining_ids.empty())
                {
                    if (accumulated_const != 0)
                    {
                        return true; // противоречие: 0 = 1
                    }
                    continue; // 0 = 0, уравнение выполнено и удаляется
                }
                // учитываем накопившуюся константу 1 (над F_2 это инверсия одного из литералов)
                if (accumulated_const != 0)
                {
                    remaining_ids[0] ^= 1;
                }
                eq.set_ids(remaining_ids);
                new_equations.push_back(eq);
            }
            // КВАДРАТИЧНОЕ УРАВНЕНИЕ (x = y AND z)
            // ids[0] = y, ids[1] = z, ids[2] = x
            else if (ids.size() == 3)
            {
                int y_raw = ids[0], z_raw = ids[1], x_raw = ids[2];
                uint8_t y_known = params[y_raw / 2].is_known();
                uint8_t y_val = y_known ? (params[y_raw / 2].get_value() ^ (y_raw % 2)) : 2;
                uint8_t z_known = params[z_raw / 2].is_known();
                uint8_t z_val = z_known ? (params[z_raw / 2].get_value() ^ (z_raw % 2)) : 2;
                uint8_t x_known = params[x_raw / 2].is_known();
                uint8_t x_val = x_known ? (params[x_raw / 2].get_value() ^ (x_raw % 2)) : 2;
                // --- ПРОВЕРКА ПРОТИВОРЕЧИЙ ---
                if (x_known && y_known && z_known)
                {
                    if (x_val != (y_val & z_val))
                    {
                        return true; // противоречие
                    }
                    changed = true;
                    continue; // уравнение выполнено, удаляем
                }
                // x = 1, но y == 0 или z == 0
                if (x_known && x_val == 1)
                {
                    if ((y_known && y_val == 0) || (z_known && z_val == 0))
                    {
                        return true; // противоречие (1 == 0 & ...)
                    }
                }
                // y == 1 или z == 1, но x уже == 0
                if ((y_known && y_val == 1) && (z_known && z_val == 1))
                {
                    if (x_known && x_val == 0)
                    {
                        return true; // противоречие
                    }
                }
                // --- ПРАВИЛА ВЫВОДА ---
                if (y_known && y_val == 1 && z_known && z_val == 1)
                {
                    literal x_lit(x_raw / 2, (x_raw % 2 == 0) ? 1 : 0);
                    if (add_literal_value(x_lit))
                    {
                        return true;
                    }
                    changed = true;
                    continue;
                }
                // rule 4: x = 1 => y = 1 и z = 1
                if (x_known && x_val == 1)
                {
                    literal y_lit(y_raw / 2, (y_raw % 2 == 0) ? 1 : 0);
                    literal z_lit(z_raw / 2, (z_raw % 2 == 0) ? 1 : 0);
                    if (add_literal_value(y_lit) || add_literal_value(z_lit))
                        return true;
                    changed = true;
                    continue;
                }
                // rule 5: y = 0 или z = 0 => x = 0
                if ((y_known && y_val == 0) || (z_known && z_val == 0))
                {
                    literal x_lit(x_raw / 2, (x_raw % 2 == 0) ? 0 : 1);
                    if (add_literal_value(x_lit))
                        return true;
                    changed = true;
                    continue;
                }
                // rule 6: x = 0 и y = 1 => z = 0
                if (x_known && x_val == 0 && y_known && y_val == 1)
                {
                    literal z_lit(z_raw / 2, (z_raw % 2 == 0) ? 0 : 1);
                    if (add_literal_value(z_lit))
                        return true;
                    changed = true;
                    continue;
                }
                // rule 7: x = 0 и z = 1 => y = 0
                if (x_known && x_val == 0 && z_known && z_val == 1)
                {
                    literal y_lit(y_raw / 2, (y_raw % 2 == 0) ? 0 : 1);
                    if (add_literal_value(y_lit))
                        return true;
                    changed = true;
                    continue;
                }
                // rule 2: y = 1 => equation превращается в x = z (линейное: x XOR z = 0)
                if (y_known && y_val == 1)
                {
                    equation lin_eq;
                    lin_eq.set_Is_line(true);
                    lin_eq.set_ids({x_raw, z_raw});
                    new_equations.push_back(lin_eq);
                    changed = true;
                    continue;
                }
                // rule 3: z = 1 => equation превращается в x = y (линейное: x XOR y = 0)
                if (z_known && z_val == 1)
                {
                    equation lin_eq;
                    lin_eq.set_Is_line(true);
                    lin_eq.set_ids({x_raw, y_raw});
                    new_equations.push_back(lin_eq);
                    changed = true;
                    continue;
                }
                // Если ни одно правило не сработало, оставляем квадратичное уравнение
                new_equations.push_back(eq);
            }
            else
            {
                new_equations.push_back(eq);
            }
        }
        equations = std::move(new_equations);
    } while (changed);
    return false;
}

void MQS_system::add_equation(equation eq)
{
    equations.push_back(eq);
    for (int i = 0; i < eq.ids.size(); i++)
    {
        if ((eq.ids[i] | 1) > params.size())
        {
            params.resize((eq.ids[i] | 0) + 2);
        }
    }
}

bool MQS_system::is_linear()
{
    for (int i = 0; i < equations.size(); i++)
    {
        if (equations[i].is_linear() == 0)
        {
            return 0;
        }
    }
    return 1;
}

bool MQS_system::add_literal_value(literal new_lit)
{
    if (params[new_lit.get_id()].is_known() == 1)
    {
        return 1;
    }
    if (params[new_lit.get_id() ^ 1].is_known() == 1)
    {
        params[new_lit.get_id() ^ 1] = literal(new_lit.get_id() ^ 1, new_lit.get_value() ^ 1);
        return 1;
    }

    params[new_lit.get_id()] = new_lit;
    params[new_lit.get_id() ^ 1] = literal(new_lit.get_id() ^ 1, new_lit.get_value() ^ 1);

    return 0;
}

bool MQS_system::add_literal_values()
{
    for (int i = 2; i < params.size(); i++)
    {
        if (params[i].is_known() == 1 and params[i ^ 1].is_known() == 1 and params[i].get_value() == params[i ^ 1].get_value())
        {
            return 1;
        }
        if (params[i].is_known() == 1 and params[i ^ 1].is_known() == 0)
        {
            params[i ^ 1] = literal(i ^ 1, params[i].get_value() ^ 1);
        }
    }

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
        if (indices.empty())
        {
            return -1;
        }
        else
        {
            return indices.front();
        }
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
        const auto &raw_ids = eq.get_ids();
        for (int raw_id : raw_ids)
        {
            int var_id = raw_id / 2; //  x_i имеет индекс i = raw_id / 2
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
    size_t total_vars = 0;
    for (const auto &row : sparse_rows)
    {
        if (!row.indices.empty())
        {
            total_vars = std::max(total_vars, static_cast<size_t>(row.indices.back() + 1));
        }
    }
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

MQS_system create(std::string filepath)
{
    // создаём поток ввода
    std::ifstream is(filepath);

    // множество всех переменных
    std::set<int> all_vars_set;

    // читаем заголовок
    std::string header;
    is >> header;

    int vars_cnt, input_vars_cnt, latches_cnt, output_vars_cnt, and_equations_cnt;

    if (header == "aag")
    {
        is >> vars_cnt >> input_vars_cnt >> latches_cnt >> output_vars_cnt >> and_equations_cnt;
    }
    else
    {
        throw std::logic_error((std::string) "wrong format: \'aag\' expected but \'" + header + "\' found");
    }

    // читаем номера входных переменных (литералов)
    std::vector<int> input_vars;
    input_vars.resize(input_vars_cnt);

    for (int i = 0; i < (int)input_vars_cnt; ++i)
    {
        is >> input_vars[i];
        all_vars_set.insert(input_vars[i] & -2);
    }

    // читаем номера выходных переменных
    std::vector<int> output_vars;
    output_vars.resize(output_vars_cnt);

    for (int i = 0; i < output_vars_cnt; ++i)
        is >> output_vars[i];

    // читаем список квадратичных уравнений
    std::vector<equation> all_equations;
    all_equations.resize(and_equations_cnt);

    for (int i = 0; i < and_equations_cnt; ++i)
    {
        int x, y, z;
        is >> x >> y >> z;
        equation eq(0, {std::min(y, z), std::max(y, z), x});
        all_equations[i] = eq;
        all_vars_set.insert(x & -2);
    }

    is >> header;

    if (header != "lin")
    {
        throw std::logic_error((std::string) "wrong format: \'lin\' expected but \'" + header + "\' found");
    }

    int linear_cnt;
    is >> linear_cnt;

    int line_cnt = 0;
    std::string line;
    std::stringstream ss;

    while (line_cnt < linear_cnt && getline(is, line))
    {
        ss.clear();
        ss << line;

        int x, rem = 0;
        std::vector<int> lits;

        while (ss >> x)
        {
            lits.push_back(x & -2);
            rem ^= x & 1;
            all_vars_set.insert(x & -2);
        }

        if (lits.empty())
            continue;

        sort(lits.begin(), lits.end());
        lits[0] ^= rem;

        equation eq(1, lits);
        all_equations.push_back(eq);
        ++line_cnt;
    }

    if (linear_cnt != line_cnt)
        std::cerr << "warning: " << "wrong linear constraints number: "
                  << linear_cnt << " expected but " << line_cnt << " found" << std::endl;

    MQS_system mqs(all_equations);
    mqs.input_vars = input_vars;   // Записываем списки входов
    mqs.output_vars = output_vars; // Записываем списки выходов

    return mqs;
}
