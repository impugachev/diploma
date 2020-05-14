double procDelta = 0, delta = 0;
do
{
    if (world.rank() != 0)
    {
        procDelta = 0;
        auto j = 0, i = gridSize - world.rank();
        double iterationDelta;
        boost::container::static_vector<mpi::request, 3> requests;
        ublas::vector<double> tempVector1{blockSize}, tempVector2{blockSize};
        // row
        for (; j < world.rank() - 1; ++j)
        {
            auto currentBlock = 
                ublas::subrange(matrix,
                    i * blockSize, (i + 1) * blockSize + 2,
                    j * blockSize + 1, (j + 1) * blockSize + 1);
            requests.clear();
            if (i != 0)
            {
                // send old top row
                requests.push_back(
                    world.isend(world.rank() + 1, MakeMessageTag("orow"), 
                        ublas::vector<double>{ublas::row(currentBlock, 1)}));
                // receive top row
                requests.push_back(
                    world.irecv(world.rank() + 1, MakeMessageTag("nrow"), tempVector1));
            }
            // receive old bottom row
            requests.push_back(
                world.irecv(world.rank() - 1, MakeMessageTag("orow"), tempVector2));
            mpi::wait_all(requests.begin(), requests.end());
            if (i != 0)
            {
                ublas::row(currentBlock, 0) = tempVector1;
            }
            ublas::row(currentBlock, currentBlock.size1() - 1) = tempVector2;
            iterationDelta = GaussSeidel(ublas::subrange(matrix,
                i * blockSize, (i + 1) * blockSize + 2,
                j * blockSize, (j + 1) * blockSize + 2), h);
            if (iterationDelta > procDelta)
            {
                procDelta = iterationDelta;
            }
            if (i != gridSize - 1)
            {
                // send bottom row
                mpi::request request =
                    world.isend(world.rank() - 1, MakeMessageTag("nrow"),
                        ublas::vector<double>{ublas::row(currentBlock, currentBlock.size1() - 2)});
                request.wait();
            }
        }
        // angle
        {
            auto currentBlockRow = 
                ublas::subrange(matrix,
                    i * blockSize, (i + 1) * blockSize + 1,
                    j * blockSize + 1, (j + 1) * blockSize + 1);
            requests.clear();
            if (i != 0)
            {
                // send old top row
                requests.push_back(
                    world.isend(world.rank() + 1, MakeMessageTag("orow"),
                        ublas::vector<double>{ublas::row(currentBlockRow, 1)}));
                // receive top row
                requests.push_back(
                    world.irecv(world.rank() + 1, MakeMessageTag("nrow"), tempVector1));
            }
            if (j != gridSize - 1)
            {
                // receive old right column
                requests.push_back(
                    world.irecv(world.rank() + 1, MakeMessageTag("ocol"), tempVector2));
            }
            mpi::wait_all(requests.begin(), requests.end());
            if (i != 0)
            {
                ublas::row(currentBlockRow, 0) = tempVector1;
            }
            auto currentBlockColumn = 
                ublas::subrange(matrix,
                    i * blockSize + 1, (i + 1) * blockSize + 1,
                    j * blockSize + 1, (j + 1) * blockSize + 2);
            if (j != gridSize - 1)
            {
                ublas::column(currentBlockColumn, currentBlockColumn.size2() - 1) = tempVector2;
            }
            iterationDelta = GaussSeidel(ublas::subrange(matrix,
                i * blockSize, (i + 1) * blockSize + 2,
                j * blockSize, (j + 1) * blockSize + 2), h);
            if (iterationDelta > procDelta)
            {
                procDelta = iterationDelta;
            }
            if (j != gridSize - 1)
            {
                // send right column
                mpi::request request =
                    world.isend(world.rank() + 1, MakeMessageTag("ncol"),
                        ublas::vector<double>{ublas::column(currentBlockColumn, blockSize - 2)});
                request.wait();
            }
            ++i;
        }
        // column
        for (; i < gridSize; ++i)
        {
            auto currentBlock = ublas::subrange(matrix,
                    i * blockSize + 1, (i + 1) * blockSize + 1,
                    j * blockSize, (j + 1) * blockSize + 2);
            requests.clear();
            // send old left column
            requests.push_back(
                world.isend(world.rank() - 1, MakeMessageTag("ocol"),
                    ublas::vector<double>{ublas::column(currentBlock, 1)}));
            // receive left column
            requests.push_back(
                world.irecv(world.rank() - 1, MakeMessageTag("ncol"), tempVector1));
            if (j != gridSize - 1)
            {
                // receive old right column
                requests.push_back(
                    world.irecv(world.rank() + 1, MakeMessageTag("ocol"), tempVector2));
            }
            mpi::wait_all(requests.begin(), requests.end());
            ublas::column(currentBlock, 0) = tempVector1;
            if (j != gridSize - 1)
            {
                ublas::column(currentBlock, currentBlock.size2() - 1) = tempVector2;
            }
            iterationDelta = GaussSeidel(ublas::subrange(matrix,
                i * blockSize, (i + 1) * blockSize + 2,
                j * blockSize, (j + 1) * blockSize + 2), h);
            if (iterationDelta > procDelta)
            {
                procDelta = iterationDelta;
            }
            if (j != gridSize - 1)
            {
                // send right column
                mpi::request request =
                    world.isend(world.rank() + 1, MakeMessageTag("ncol"),
                        ublas::vector<double>{ublas::column(currentBlock, currentBlock.size1() - 2)});
                request.wait();
            }
        }
    }
    mpi::all_reduce(world, procDelta, delta, mpi::maximum<double>());
} while (delta > eps);