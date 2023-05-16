#include <array>
#include <vector>
#include <cmath>
#include <iostream>
#include <cmath>
#include <mpi.h>
#include <fstream>
#include <cassert>
#include <memory.h>
#include <unistd.h>

constexpr double a = 1.0;
constexpr double X = 50.0;
constexpr double T = 1.0;

double func(double x, double t) { 
    return std::exp(std::sin(x * t / X));
}

double phi(double x) {
    return std::cos(M_PI * x / X);
}

double psi(double t) {
    return std::exp(-t);
}


// For the method to be stable, Courant number must be less than 1
// a * tau / h < 1
constexpr double h = 0.05;
constexpr double tau = 0.001;
constexpr size_t K = T / tau;
constexpr size_t M = X / h;

class Worker {
public:
    Worker(int rank, int commsize) {
        rank_ = rank;
        commsize_ = commsize;

        CalculateStartAndEndPos();
        values_ = new double[cols_ * K_] {};

        if (rank_ == 0) {
            if (commsize_ > 1) {
                result_values_ = new double[M_ * K_] {}; 
            } else {
                result_values_ = values_;
            }
        }
    }

    ~Worker() {
        delete[] values_;
        values_ = nullptr;

        if (rank_ == 0 && commsize_ > 1) {
            delete[] result_values_;
            result_values_ = nullptr;
        }
    }

    void FillInitialConditions();
    void CalculateFirstLineByRectangleMethod();
    void CalculateOtherLinesByCrossMethod();
    void GatheringAllWork();
    void PrintResult();

private:
    size_t rank_ {0};
    size_t commsize_ {0};
    size_t start_pos_ {0};
    size_t end_pos_ {0};
    size_t cols_ {0};

    size_t M_ {M};
    size_t K_ {K};

    double* values_ {nullptr};
    double* result_values_ {nullptr};

    void CalculateStartAndEndPos();
};


int main(int argc, char** argv) {
    MPI::Init(argc, argv);

    auto rank = MPI::COMM_WORLD.Get_rank();
    auto commsize = MPI::COMM_WORLD.Get_size();

    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(2 * rank, &mask);
    sched_setaffinity(getpid(), sizeof(cpu_set_t), &mask);

    Worker result(rank, commsize);
    
    double start_wtime = MPI::Wtime();
    
    result.FillInitialConditions();
    result.CalculateFirstLineByRectangleMethod();
    result.CalculateOtherLinesByCrossMethod();

    if (commsize > 1) {
        result.GatheringAllWork();
    }

    double end_wtime = MPI::Wtime();

    if (rank == 0) {
        std::cout << "exec_time: " << end_wtime - start_wtime << std::endl;
        result.PrintResult();
    }

    MPI::Finalize();
    return 0;
}

void Worker::FillInitialConditions()
{
    if (rank_ == 0) {
        for (size_t i = 0; i < K_; ++i) {
            values_[0 + i * cols_] = psi(tau * i);
        }
    }
    for (size_t i = 0; i < cols_; ++i) {
        values_[i] = phi(h * (i + start_pos_));
    }
}

void Worker::CalculateFirstLineByRectangleMethod()
{
    if (rank_ == 0) {
        for (size_t m = 1; m < cols_; ++m) {
            double first_part = (values_[cols_ + m - 1] - values_[m - 1] - values_[m]) / (2 * tau);
            double second_part = a * (-1.0 * values_[cols_ + m - 1] + values_[m]  - values_[m - 1]) / (2 * h);
            double func_part = func((m + 0.5) * h, (1 + 0.5) * tau);
            values_[cols_ + m] = (func_part - first_part - second_part) * 2 * tau * h / (h + a * tau);
        }
        if (commsize_ > 1) {
            MPI_Send(values_ + cols_ + cols_ - 1, 1, MPI::DOUBLE, rank_ + 1, 0, MPI_COMM_WORLD);
            MPI_Send(values_ + cols_ - 1, 1, MPI::DOUBLE, rank_ + 1, 0, MPI_COMM_WORLD);
        }
        return;
    }

    double recv_up_value = 0;
    double recv_down_value = 0;
    MPI_Status status;
    MPI_Recv(&recv_up_value, 1, MPI::DOUBLE, rank_ - 1, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(&recv_down_value, 1, MPI::DOUBLE, rank_ - 1, 0, MPI_COMM_WORLD, &status);
    
    for (size_t m = 0; m < cols_; ++m) {
        if (m != 0) {
            recv_up_value = values_[cols_ + m - 1];
            recv_down_value = values_[m - 1];
        }
        double first_part = (recv_up_value - recv_down_value - values_[m]) / (2 * tau);
        double second_part = a * (-1.0 * recv_up_value + values_[m]  - recv_down_value) / (2 * h);
        double func_part = func((start_pos_ + m + 0.5) * h, (1 + 0.5) * tau);
        values_[cols_ + m] = (func_part - first_part - second_part) * 2 * tau * h / (h + a * tau);
    }
    if (rank_ != commsize_ - 1) {
        MPI_Send(values_ + cols_ + cols_ - 1, 1, MPI::DOUBLE, rank_ + 1, 0, MPI_COMM_WORLD);
        MPI_Send(values_ + cols_ - 1, 1, MPI::DOUBLE, rank_ + 1, 0, MPI_COMM_WORLD);
    }
}

void Worker::CalculateOtherLinesByCrossMethod()
{
    for (size_t k = 1; k < K_ - 1; ++k) {
        double recv_left_value = 0; 
        double recv_right_value = 0;
        MPI_Status status;

        if (rank_ != 0) {
            // send value to left
            int buf_size = sizeof(double) + MPI_BSEND_OVERHEAD;
            double* buf = new double[buf_size];
            MPI_Buffer_attach(buf, buf_size);
            MPI_Bsend(values_ + cols_ * k, 1, MPI::DOUBLE, rank_ - 1, 0, MPI_COMM_WORLD);
            MPI_Buffer_detach(&buf, &buf_size);
            delete[] buf;
        }
        if (rank_ != commsize_ - 1) {
            // send value to right
            int buf_size = sizeof(double) + MPI_BSEND_OVERHEAD;
            double* buf = new double[buf_size];
            MPI_Buffer_attach(buf, buf_size);
            MPI_Bsend(values_ + cols_ * k + cols_ - 1, 1, MPI::DOUBLE, rank_ + 1, 0, MPI_COMM_WORLD);
            MPI_Buffer_detach(&buf, &buf_size);
            delete[] buf;
        }

        // if rank = 0 we dont need to calculate m = 0
        for (size_t m = (rank_ == 0); m < cols_; ++m) {
            if (m != 0) {
                recv_left_value = values_[cols_ * k + m - 1];
            } else if (rank_ != 0) {
                // recv value from left
                MPI_Recv(&recv_left_value, 1, MPI::DOUBLE, rank_ - 1, 0, MPI_COMM_WORLD, &status);
            }
            if (m != cols_ - 1) {
                recv_right_value = values_[cols_ * k + m + 1];
            } else if (rank_ != commsize_ - 1) {
                // recv value from right
                MPI_Recv(&recv_right_value, 1, MPI::DOUBLE, rank_ + 1, 0, MPI_COMM_WORLD, &status);
            }

            if (m == cols_ - 1 && rank_ == commsize_ - 1) {
                // rectangle method for case of last worker
                double first_part = (values_[cols_ * (k + 1) + m - 1] - values_[cols_ * k + m - 1] - values_[cols_ * k + m]) / (2 * tau);
                double second_part = a * (-1.0 * values_[cols_ * (k + 1) + m - 1] + values_[cols_ * k + m] - values_[cols_ * k + m - 1]) / (2 * h);
                double func_part = func((start_pos_ + m + 0.5) * h, (k + 0.5) * tau);
                values_[cols_ * (k + 1) + m] = (func_part - first_part - second_part) * 2 * tau * h / (h + a * tau);
            } else {
                // cross method in other cases
                double first_part = (-1.0 * values_[cols_ * (k - 1) + m]) / (2 * tau);
                double second_part = a * (recv_right_value - recv_left_value) / (2 * h);
                double func_part = func((start_pos_ + m) * h, k * tau);
                values_[cols_ * (k + 1) + m] = (func_part - first_part - second_part) * 2 * tau;
            }
        }
    }
}

void Worker::PrintResult()
{
    std::string name = "./result.dat";
    std::ofstream ost(name);

    ost << "tau: " << tau << std::endl;
    ost << "h: " << h << std::endl;
    ost << "T: " << T << std::endl;
    ost << "X: " << X << std::endl;
    ost << "M: " << M_ << std::endl;
    ost << "K: " << K_ << std::endl;

    double* array_to_print = nullptr;
    if (commsize_ == 1) {
        array_to_print = values_;
    } else {
        array_to_print = result_values_;
    }

    for (size_t i = 0, size = K_ * M_; i < size; ++i) {
        ost << array_to_print[i] << std::endl;
    }
}

void Worker::CalculateStartAndEndPos()
{
    size_t distance = M_ / commsize_;
    size_t remainder = M_ % commsize_;
    if (remainder != 0) {
        M_ = (distance + 1) * commsize_;
        ++distance;
    }
    start_pos_ = distance * rank_;
    end_pos_ = distance * (rank_ + 1);

    cols_ = end_pos_ - start_pos_;
}

void Worker::GatheringAllWork()
{    
    for (size_t k = 0; k < K_; ++k) {
        MPI_Gather(values_ + k * cols_, cols_, MPI::DOUBLE, result_values_ + k * M_, cols_, MPI::DOUBLE, 0, MPI_COMM_WORLD);
    }
}