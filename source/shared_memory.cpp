#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <string>
#include <cstdint>

int main(int argc, char *argv[])
{
    using namespace boost::interprocess;

    if (argc == 1) // если процесс - родитель
    {
        // элементарная RAII-обертка
        struct shm_remove
        {
            shm_remove() { shared_memory_object::remove("MySharedMemory"); }
            ~shm_remove() { shared_memory_object::remove("MySharedMemory"); }
        } remover;
        // создаем общую память
        shared_memory_object shm(create_only, "MySharedMemory", read_write);
        // устанавливаем ее размер
        shm.truncate(1000);
        // отображаем общую память в память процесса
        mapped_region region(shm, read_write);
        // пишем единицы в общую память
        std::memset(region.get_address(), 1, region.get_size());
        // запускаем дочерний процесс
        std::string s(argv[0]);
        s += " child ";
        if (std::system(s.c_str()) != 0)
            return 1; // не удалось создать процесс
    }
}