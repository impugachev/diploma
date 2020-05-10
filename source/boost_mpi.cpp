#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>

#include <iostream>

int main(int argc, char* argv[])
{
    mpi::environment env;
    mpi::communicator world;

    if (world.rank() == 0)
    {
        std::cout << "I’m master processor" << std::endl;
    }
    else
    {
        std::cout << "I'm " << world.rank() << " processor" << std::endl;
    }
    
    return 0;
}
