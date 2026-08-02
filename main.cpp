#include <iostream>
#include <string>
#include <fstream>

#include "search_tree.hpp"
#include "equations_system.hpp"
#include "linearizing_set.hpp"

int main(int argc, char *argv[])
{
    std::string filename;
    std::string algorithm = "jump";
    std::string heuristic = "often";
    bool showHelp = false;

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        if (arg == "-a" && i + 1 < argc)
        {
            int nextIndex = i + 1;
            algorithm = argv[nextIndex];
            i = nextIndex;
        }
        else if (arg == "-h" && i + 1 < argc)
        {
            int nextIndex = i + 1;
            heuristic = argv[nextIndex];
            i = nextIndex;
        }
        else if (arg == "--help")
        {
            showHelp = true;
        }
        else if (filename.empty() && arg[0] != '-')
        {
            filename = arg;
        }
        else
        {
            std::cout << "Unknown parameter: " << arg << std::endl;
            return 1;
        }
    }

    if (showHelp || filename.empty())
    {
        std::cout << "========================================" << std::endl;
        std::cout << "MQS Solver - solve quadratic equations" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "  " << argv[0] << " <file.mqs> [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Required:" << std::endl;
        std::cout << "  <file.mqs>    file with equations" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -a <algo>     algorithm: simple, jump, smart" << std::endl;
        std::cout << "                 (default: jump)" << std::endl;
        std::cout << "  -h <heur>     heuristic: often, rare" << std::endl;
        std::cout << "                 (default: often)" << std::endl;
        std::cout << "  --help        show this help" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  " << argv[0] << " encodings/test1.mqs" << std::endl;
        std::cout << "  " << argv[0] << " encodings/test1.mqs -a smart" << std::endl;
        std::cout << "  " << argv[0] << " encodings/test1.mqs -a jump -h rare" << std::endl;

        if (filename.empty())
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    std::cout << "=== PARAMETERS ===" << std::endl;
    std::cout << "File:       " << filename << std::endl;
    std::cout << "Algorithm:  " << algorithm << std::endl;
    std::cout << "Heuristic:  " << heuristic << std::endl;
    std::cout << std::endl;

    std::cout << "Reading file: " << filename << " ..." << std::endl;

    // std::ifstream file(filename);
    // if (!file.is_open())
    // {
    //     std::cout << "ERROR: cannot open file!" << std::endl;
    //     return 1;
    // }

    // std::string word;
    // int vars, eqs;
    // file >> word >> vars;
    // file >> word >> eqs;

    MQS_system system = create(filename);
    // std::cout << "=== DEBUG PARSER ===" << std::endl;
    // std::cout << "params.size() = " << system.params.size() << std::endl;
    // for (size_t i = 0; i < system.params.size(); ++i)
    // {
    //     if (system.params[i].is_known())
    //     {
    //         std::cout << "params[" << i << "] is KNOWN, val = " << (int)system.params[i].get_value() << std::endl;
    //     }
    // }
    // std::cout << "Equations count: " << system.equations.size() << std::endl;
    // for (const auto &eq : system.equations)
    // {
    //     std::cout << "Eq ids: ";
    //     for (int id : eq.get_ids())
    //         std::cout << id << " ";
    //     std::cout << std::endl;
    // }
    // std::cout << "Found " << system.params.size() / 2 - 1 << " variables and " << system.equations.size() << " equations" << std::endl;
    std::cout << std::endl;

    std::cout << "Algorithm: ";
    if (algorithm == "simple")
    {
        std::cout << "SIMPLE BACKTRACKING" << std::endl;
    }
    else if (algorithm == "jump")
    {
        std::cout << "BACKTRACKING WITH BACKJUMPING" << std::endl;
    }
    else if (algorithm == "smart")
    {
        std::cout << "SMART SEARCH (CDCL-like)" << std::endl;
    }
    else
    {
        std::cout << "UNKNOWN!" << std::endl;
        std::cout << "Available: simple, jump, smart" << std::endl;
        return 1;
    }

    std::cout << "Heuristic: ";
    if (heuristic == "often")
    {
        std::cout << "MOST FREQUENT VARIABLE" << std::endl;
    }
    else if (heuristic == "rare")
    {
        std::cout << "LEAST FREQUENT VARIABLE" << std::endl;
    }
    else
    {
        std::cout << "UNKNOWN!" << std::endl;
        std::cout << "Available: often, rare" << std::endl;
        return 1;
    }
    std::cout << std::endl;
    // до тяжелого обзлда проверяем логику (правила вывода)
    literal dummy(0, 0);
    system.unit_propagation(dummy);

    std::cout << "=== AFTER PROPAGATION ===" << std::endl;
    for (size_t i = 1; i < system.params.size() / 2; ++i)
    {
        int raw_id = 2 * i;
        std::cout << "x" << i << " = "
                  << (system.params[raw_id].is_known() ? std::to_string(system.params[raw_id].get_value()) : "UNKNOWN")
                  << std::endl;
    }
    std::cout << "Equations remaining: " << system.equations.size() << std::endl;
    SearchTree tree(system);
    std::cout << "=== STARTING SEARCH ===" << std::endl;
    if (tree.solve())
    {
        tree.printSolution();
    }
    else
    {
        std::cout << "UNSAT: No solution found!" << std::endl;
    }
    return 0;
}
