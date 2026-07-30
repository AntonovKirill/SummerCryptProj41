#ifndef EQUATIONS_SYSTEM
#define EQUATIONS_SYSTEM

#include <iostream>  //console (in/out) stream
#include <cstdint>   //std::uint8_t
#include <string>    //std::string
#include <vector>    //std::vector
#include <map>       //std::map
#include <algorithm> //std::swap
#include <cmath>     //std::pow

class literal
{
public:
    int id;             // Идентификационный номер литерала равный 2i для x_i и 2i+1 для NOT(x_i).
    std::uint8_t value; // Найденное в ходе решения значение.


    literal();
    literal(int id0);
    literal(int id0, std::uint8_t value0);
    /*
            literal* create();
            literal* create(int id0);
            literal* create(int id0, std::uint8_t value0);
            //Функции выше предназначены для корректного создания создания объекта литерала.
    */
    ~literal();
    void destroy();
    // Функция предназначена для корректного удаления объекта литерала.

    void set_value(std::uint8_t value0);

    std::uint8_t get_value() const;
    int get_id();

    bool is_known();
    // Функция предназаначена для проверки заданныли значения литерала.
    // Возвращает 1 если значеничение литерала уже было инициализровано ранее.
    // Возвращает 0 в противном случае.
};

class equation
{
public:
    bool Is_line;         // В рамках данной работы уравнения могут быть только линейными или квадратичные.
    std::vector<int> ids; // Задает порядок литералов в уравнении.
    /*
    Переменные выше в рамках данной работы одназначно определяют любое возможное уравнение.

    Пример 1:

        Is_line = 1 (линейное)
        param_form = {
            -> id=2
            -> id=3
            -> id=7
        }

        x1 XOR (x1 XOR 1) XOR (x3 XOR 1) = 0

    Пример 2:

        Is_line = 0 (Квадратичное)
        param_form = {
            -> id=2
            -> id=4
            -> id=6
        }

        x1 AND x2 XOR x3 = 0
    */

    equation();
    equation(bool Is_line0);
    equation(const std::vector<int> &ids0);
    equation(bool Is_line0, const std::vector<int> &ids0);
    /*
        equation* create(Is_line0, param_form0);
        // Функция выше предназначена для корректного создания создания объекта уравнения.
    */
   
    ~equation();
    void destroy();
    // Функция предназначена для корректного удаления объекта уравнения.

    void set_Is_line(bool Is_line0);
    void set_ids(std::vector<int> ids0);

    bool get_Is_line() const;
    std::vector<int> get_ids() const;

    bool is_linear();
    // Эта функция предназначена для решения системы (и уравнений в частности), реализации логики графа.
    // Вернет 1 в случае если уравнение линейно.
    // Вернет 0 в противном случае.
};

class MQS_system
{
public:
    // TODO: разобраться, почему в комментарии сказано про указатели, а на деле просто объект
    std::vector<literal> params;     // Массив указателей литералы используемые системы (и сопряженые им)
    std::vector<equation> equations; // Массив уравнений системы

    MQS_system();
    MQS_system(std::vector<equation> equations0);
    /*
        MQS_system* create(std::string filepath);
        // Функция выше предназначены для корректного создания объекта системы.
    */

    ~MQS_system();
    void destroy();
    // Эта функция предназначена для корректного удаления объекта системы.

    void set_params(std::vector<literal> params0);
    void set_equations(std::vector<equation> equations0);

    std::vector<literal> get_params() const;
    std::vector<equation> get_equations() const;

    bool unit_propagation(literal &x); // Эта функция возвращает объект системы сокращенный с использоавнием имеющихся данных.

    void add_equation(equation eq);
    // Эта функция предназначена для добавления нового уравнения в объект системы.
    //(дополнительное ограничения при получении несовместного решения).

    bool is_linear();
    // Эта функция предназначена для решения системы (и уравнений в частности), реализации логики графа.
    // Вернет 1 в случае если система линейна.
    // Вернет 0 в противном случае.

    bool add_literal_value(literal new_lit);
    // Эта функция предназначена для добавления значения одного из литералов.
    // Вернет 0 в случае успешного добавления значения литерала.
    // Вернет 1 в случае если система несовместна (Противоречие с уже имеющимися значениями).

    bool add_literal_values();
    // Эта функция предназначена для вызова из части программы отвечающей за решение уравнений.
    // Вернет 0 в случае успешного добавления значения сопряженных литералов.
    // Вернет 1 в случае если система несовместна (Противоречие с уже имеющимися значениями).

    bool solve_gauss();
    // Метод Гаусса для линейных систем над F_2.
    // Вернет 0, если система имеет решение/совместна.
    // Вернет 1, если система несовместна (противоречие).
};

MQS_system create(std::string filepath);
// Функция выше предназначены для корректного создания объекта системы через имя файла с MQS encode

#endif
