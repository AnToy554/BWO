#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

// Hàm fitness giả định cho hệ thống mạng
// Ví dụ: f(x) = c1 * băng thông + c2 * chi phí + c3 * độ trễ (tối ưu hóa băng thông và chi phí)
double fitness_function(const vector<double>& x) {
    // Giả sử:
    // x[0] là băng thông
    // x[1] là chi phí
    // x[2] là độ trễ
    
    double bandwidth = x[0];
    double cost = x[1];
    double latency = x[2];
    
    // Hàm fitness cần tối ưu hóa: giả sử ta muốn tối đa hóa băng thông, nhưng giảm thiểu chi phí và độ trễ
    double c1 = 1, c2 = 1, c3 = 1;  // Các trọng số cho băng thông, chi phí và độ trễ
    double fitness = c2 * cost + c3 * latency - c1 * bandwidth;  // Fitness có thể là một hàm kết hợp

    return fitness;
}

// Hàm tìm kiếm widow tốt nhất
pair<vector<double>, double> find_best(const vector<vector<double>>& widows, const vector<double>& fitness_values) {
    int best_index = distance(fitness_values.begin(), min_element(fitness_values.begin(), fitness_values.end()));
    return {widows[best_index], fitness_values[best_index]};
}

// Black Widow Optimization (BWO)
pair<vector<double>, double> black_widow_optimization(int N, int D, double ReproductionRate, double CannibalismRate, double MutationRate, double (*f)(const vector<double>&)) {
    // Khởi tạo dân số ngẫu nhiên (mỗi cá thể là một bộ tham số trong không gian D)
    vector<vector<double>> widows(N, vector<double>(D));
    srand(time(0));
    
    // Khởi tạo ngẫu nhiên các giá trị trong khoảng [-5, 5]
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < D; ++j) {
            widows[i][j] = (rand() / (double)RAND_MAX) * 10 - 5;
        }
    }

    // Đánh giá độ phù hợp cho mỗi widow
    vector<double> fitness_values(N);
    for (int i = 0; i < N; ++i) {
        fitness_values[i] = f(widows[i]);
    }

    // Tìm widow tốt nhất ban đầu
    auto [best_solution, best_fitness] = find_best(widows, fitness_values);

    // Tính số lượng widow sẽ sinh sản
    int N_R = int(N * ReproductionRate);

    // Vòng lặp chính
    int iteration = 0;
    int max_iterations = 100;  // Giới hạn số vòng lặp
    
    while (iteration < max_iterations) {
        iteration++;

        // Chọn các widow tốt nhất để làm cha mẹ
        vector<int> parents_indices(N_R);
        iota(parents_indices.begin(), parents_indices.end(), 0);
        sort(parents_indices.begin(), parents_indices.end(), [&fitness_values](int i, int j) {
            return fitness_values[i] < fitness_values[j];  // Sắp xếp theo giá trị fitness
        });

        // Tạo con cái
        vector<vector<double>> children(N_R, vector<double>(D));
        
        for (int i = 0; i < N_R; ++i) {
            // Chọn ngẫu nhiên 2 solutions từ các bậc cha mẹ
            int parent1_idx = parents_indices[rand() % N_R];
            int parent2_idx = parents_indices[rand() % N_R];
            
            // Tạo crossover tại điểm ngẫu nhiên
            int crossover_point = rand() % D;
            for (int j = 0; j < D; ++j) {
                children[i][j] = (j < crossover_point) ? widows[parent1_idx][j] : widows[parent2_idx][j];
            }
        }

        // Cannibalism (chọn những đứa con từ các solution tốt nhất)
        int cannibalism_count = int(CannibalismRate * N_R);
        for (int i = 0; i < cannibalism_count; ++i) {
            children[i] = widows[parents_indices[i]];  // Chọn từ các cá thể tốt nhất
        }

        // Đột biến
        for (int i = 0; i < N_R; ++i) {
            if (rand() / (double)RAND_MAX < MutationRate) {
                // Đột biến ngẫu nhiên
                for (int j = 0; j < D; ++j) {
                    children[i][j] += (rand() / (double)RAND_MAX) * 2 - 1;  // Đột biến trong khoảng [-1, 1]
                }
            }
        }

        // Cập nhật dân số mới (bao gồm cả con cái và các cá thể đột biến)
        widows.clear();
        for (int i = 0; i < N_R; ++i) {
            widows.push_back(children[i]);
        }

        // Đánh giá lại độ phù hợp
        fitness_values.clear();
        for (int i = 0; i < N_R; ++i) {
            fitness_values.push_back(f(widows[i]));
        }

        // Tìm lại widow tốt nhất
        auto [current_best_solution, current_best_fitness] = find_best(widows, fitness_values);

        // Cập nhật nếu tìm được solution tốt hơn
        if (current_best_fitness < best_fitness) {
            best_solution = current_best_solution;
            best_fitness = current_best_fitness;
        }
    }

    return {best_solution, best_fitness};
}

int main() {
    // Các tham số thuật toán
    int N = 50;  // Kích thước dân số
    int D = 3;   // Số chiều của bài toán (băng thông, chi phí, độ trễ)
    double ReproductionRate = 0.6;
    double CannibalismRate = 0.2;
    double MutationRate = 0.3;

    // Gọi thuật toán
    auto [best_solution, best_fitness] = black_widow_optimization(N, D, ReproductionRate, CannibalismRate, MutationRate, fitness_function);

    // In kết quả
    cout << "Best solution: ";
    for (const double& val : best_solution) {
        cout << val << " ";
    }
    cout << endl;
    cout << "Best fitness: " << best_fitness << endl;

    return 0;
}
