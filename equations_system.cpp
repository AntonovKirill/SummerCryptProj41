#include "equations_system.hpp"
#include <vector>
#include <algorithm> // для swap

bool MQS_system::solve_gauss()
{
    if (this->is_linear() != 0) // проверка линейности системы
    {
        return true;
    }
    size_t n_eqs = equations.size(); // кол-во уравнений в с-ме
    if (n_eqs == 0)
        return false;

    size_t n_vars = param_num.size(); // кол-во переменных в с-ме (размерность)
    std::vector<std::vector<uint8_t>> matrix(n_eqs, std::vector<uint8_t>(n_vars + 1, 0));
    // двумерный вектор размером n_eqs строк на n_vars + 1 столбцов (+1 для свободных членов)
    for (size_t i = 0; i < n_eqs; ++i)
    {
        std::vector<literal *> lits = equations[i].get_param_form(); // берем список его литералов
        for (literal *lit : lits)
        {
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
            return true;
        }
    }

    return false;
}
