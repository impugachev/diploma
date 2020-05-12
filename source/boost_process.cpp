// создание потока для данных
boost::process::ipstream stream;
// создание дочернего процесса с перенаправленным stdout в созданный поток
boost::process::child child(boost::process::search_path("my_child_process"), 
    file, boost::process::std_out > stream);

std::vector<std::string> data;
std::string line;

// чтение stdout процесса
while (child.running() && std::getline(stream, line) && !line.empty())
    data.push_back(line);
// ожидание завершения процесса
child.wait();