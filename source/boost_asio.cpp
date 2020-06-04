// создание дочернего процесса
m_process = boost::process::child(m_runBinCommand,
            boost::process::std_out > m_pipe);
// установка функции обратного вызова, которая сработает при чтении 
// символа перевода строки из потока вывода дочернего процесса
boost::asio::async_read_until(m_pipe, m_buffer, '\n', m_readHandler);
// создание потока чтения
std::thread([this, &callback]()
{
    try
    {
        m_ioService.run();
    }
    catch (boost::wrapexcept<boost::system::system_error>& e)
    {
        cmd::log << e.what() << std::endl;
    }
    m_process.join();
    if (m_process.exit_code() == 0)
        callback();
}).detach();

// функция, вызываемая при получении очередной порции данных
m_readHandler = 
    [this](const boost::system::error_code& e, std::size_t size)
    {
        // если прочитали весь канал, завершаем чтение
        if (e.failed() || size == 0)
        {
            m_stream->CloseStream();
            return;
        }
        // перенаправление прочитанных данных для отправки RPC-клиенту
        (*m_stream) << ReadPointsFromBuffer();
        // заново ставим callback на чтение
        boost::asio::async_read_until(m_pipe,m_buffer, '\n', m_readHandler);
    };