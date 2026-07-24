#include "equations_system.hpp"
#include <vector>
#include <algorithm>

bool MQS_system::solve_gauss()
{
    if (this->is_linear() != 0) // проверка линейности системы
    {
        return true;
    }
    size_t n_eqs = equations.size();
    if (n_eqs == 0)
        return false;

    size_t n_vars = param_num.size();
    std::vector<std::vector<uint8_t>> matrix(n_eqs, std::vector<uint8_t>(n_vars + 1, 0));
    for (size_t i = 0; i < n_eqs; ++i)
    {
        std::vector<literal *> lits = equations[i].get_param_form();
        for (literal *lit : lits)
        {
            int var_index = lit->get_id() / 2;
            if (var_index < n_vars)
            {
                matrix[i][var_index] ^= 1;
            }
        }
        matrix[i][n_vars] = equations[i].get_free_term();
    }

    size_t pivot_row = 0;
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
        std::swap(matrix[pivot_row], matrix[sel]);
        for (size_t r = 0; r < n_eqs; ++r)
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
            return true;
        }
    }

    return false;
}
