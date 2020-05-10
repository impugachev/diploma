double GaussSeidel(ublas::matrix_range<ublas::matrix<double>> matrix, 
    double h)
{
    const auto maxWaveSize = matrix.size1() - 2;
    std::vector<double> deltas(maxWaveSize);
    for (auto waveSize = 1; waveSize < maxWaveSize + 1; ++waveSize) 
    {
#pragma omp parallel for shared(matrix,waveSize,deltas)
        for (int i = 1; i < waveSize + 1; ++i) 
        {
            auto j = waveSize + 1 - i;
            GaussSeidelIteration(matrix, i, j, h, deltas);
        }
    }

    for (int waveSize = maxWaveSize - 1; waveSize > 0; --waveSize) 
    {
#pragma omp parallel for shared(matrix,waveSize,deltas)
        for (int i = maxWaveSize - waveSize + 1; 
            i < maxWaveSize + 1; ++i) 
        {
            auto j = 2 * maxWaveSize - waveSize - i + 1;
            GaussSeidelIteration(matrix, i, j, h, deltas);
        }
    }
    return *std::max_element(deltas.begin(), deltas.end());
}