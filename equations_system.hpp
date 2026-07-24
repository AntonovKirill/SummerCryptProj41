#ifndef EQUATIONS_SYSTEM
#define EQUATIONS_SYSTEM

#include <iostream> //console (in/out) stream
#include <cstdint>  //std::uint8_t
#include <string>   //std::string
#include <vector>   //std::vector

class literal
{
private:
    int id;     // Идентификационный номер литерала равный 2i для x_i и 2i+1 для NOT(x_i).
    char value; // Найденное в ходе решения значение.

    literal();
    literal(int id);
    literal(int id, char value);

    ~literal();

    void set_value(char value);

    char get_value() const;

public:
    literal *create();
    literal *create(int id);
    literal *create(int id, char value);
    // Функции выше предназначены для корректного создания создания объекта литерала.

    void destroy();
    // Функция предназначена для корректного удаления объекта литерала.

    bool is_known();
    // Функция предназаначена для проверки заданныли значения литерала.
    // Возвращает 0 если значеничение литерала уже было инициализровано ранее.
    // Возвращает 1 в противном случае.
};

class equation
{
private:
    bool Is_line;                      // В рамках данной работы уравнения могут быть только линейными или квадратичные.
    std::vector<literal *> param_form; // Задает порядок литералов в уравнении.
    /*Переменные выше в рамках данной работы одназначно определяют любое возможное уравнение.

    Пример 1:

        Is_line = 0 (линейное)
        param_form = {
            -> id=2
            -> id=3
            -> id=7
        }

        x1 XOR (x1 XOR 1) XOR (x3 XOR 1) = 0

    Пример 2:

        Is_line = 1 (Квадратичное)
        param_form = {
            -> id=2
            -> id=4
            -> id=6
        }

        x1 AND x2 XOR x3 = 0
    */

    equation();
    equation(param_form);
    equation(Is_line, param_form);

    ~equation();

    void set_Is_line(bool Is_line);
    void set_param_form(std::vector<literal> param_form);

    bool get_Is_line() const;
    std::vector<literal> get_param_form() const;

public:
    equation *create(param_form);
    equation *create(Is_line, param_form);
    // Функции выше предназначены для корректного создания создания объекта уравнения.

    void destroy();
    // Функция предназначена для корректного удаления объекта уравнения.

    bool is_linear();
    // Эта функция предназначена для решения системы (и уравнений в частности), реализации логики графа.
    // Вернет 0 в случае если уравнение линейно.
    // Вернет 1 в противном случае.
};

class MQS_system
{
private:
    std::vector<literal *> param_num; // Массив указателей литералы используемые системы (и сопряженые им)
    std::vector<equation> equations;  // Массив уравнений системы

    MQS_system();
    MQS_system(std::vector<equation> equations);

    ~MQS_system();

    void set_param_num(std::vector<literal> param_num);
    void set_equations(std::vector<equation> equations);

    std::vector<literal> get_param_num() const;
    std::vector<equation> get_equations() const;

public:
    MQS_system *create(std::string filepath);
    MQS_system *create(std::vector<equation> equations);
    // Функции выше предназначены для корректного создания объекта системы.

    void destroy();
    // Эта функция предназначена для корректного удаления объекта системы.

    MQS_system unit_propagation();
    // Эта функция возвращает объект системы сокращенный с использоавнием имеющихся данных.

    void add_equation(equation eq);
    // Эта функция предназначена для добавления нового уравнения в объект системы.
    //(дополнительное ограничения при получении несовместного решения).

    bool is_linear();
    // Эта функция предназначена для решения системы (и уравнений в частности), реализации логики графа.
    // Вернет 0 в случае если система линейна.
    // Вернет 1 в противном случае.

    bool add_literal_value();
    // Эта функция предназначена для вызова из части программы отвечающей за решение уравнений.
    // Вернет 0 в случае успешного добавления значения сопряженных литералов.
    // Вернет 1 в случае если система несовместна (Противоречие с уже имеющимися значениями).

    bool solve_gauss();
    // Метод Гаусса для линейных систем над F_2.
    // Вернет 0, если система имеет решение/совместна.
    // Вернет 1, если система несовместна (противоречие).
};

#endif
