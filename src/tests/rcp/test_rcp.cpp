#include "csympy_rcp.h"

using CSymPy::RCP;
using CSymPy::Ptr;
using CSymPy::rcp;

int main(int argc, char* argv[])
{

    Ptr<int> a;
    {
        RCP<int> b = rcp(new int(1));
        *b = 5;
        a = b.ptr();
        std::cout << *a << std::endl;
        std::cout << *b << std::endl;
    }
    std::cout << *a << std::endl;
    *a = 5;

    return 0;
}
