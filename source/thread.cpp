double GaussSeidel(ublas::matrix_range<ublas::matrix<double>> matrix, 
    double h)
{
    const auto maxWaveSize = matrix.size1() - 2;
    std::vector<double> deltas(maxWaveSize);
    std::vector<std::thread> threads;
    for (auto waveSize = 1; waveSize < maxWaveSize + 1; ++waveSize) 
    {
        for (int i = 1; i < waveSize + 1; ++i) 
        {
            auto j = waveSize + 1 - i;
            threads.emplace_back(GaussSeidelIteration, 
                matrix, i, j, h, deltas);
        }
        for (auto& thread : threads)
        {
            thread.join();
        }
        threads.clear();
    }
    for (int waveSize = maxWaveSize - 1; waveSize > 0; --waveSize) 
    {
        for (int i = maxWaveSize - waveSize + 1; i < maxWaveSize + 1; ++i) 
        {
            auto j = 2 * maxWaveSize - waveSize - i + 1;
            threads.emplace_back(GaussSeidelIteration, 
                matrix, i, j, h, deltas);
        }
        for (auto& thread : threads)
        {
            thread.join();
        }
        threads.clear();
    }
    return *std::max_element(deltas.begin(), deltas.end());
}