import numpy as np

def black_widow_optimization(N, D, ReproductionRate, CannibalismRate, MutationRate, f):
    # Khởi tạo dân số ngẫu nhiên (mỗi cá thể là một bộ tham số trong không gian D)
    # Ở đây D có thể đại diện cho các tham số như băng thông, chi phí, độ trễ,...
    widows = np.random.uniform(low=-5, high=5, size=(N, D))  # Giới hạn giá trị tham số trong khoảng [-5, 5]
    
    # Đánh giá độ phù hợp cho mỗi widow
    fitness_values = np.apply_along_axis(f, 1, widows)
    
    # Hàm tìm kiếm widow tốt nhất
    def find_best(widows, fitness_values):
        best_index = np.argmin(fitness_values)  # Tìm widow có fitness thấp nhất (vì fitness có thể là chi phí)
        return widows[best_index], fitness_values[best_index]
    
    # Tìm widow tốt nhất ban đầu
    best_solution, best_fitness = find_best(widows, fitness_values)

    # Tính số lượng widow sẽ sinh sản
    N_R = int(N * ReproductionRate)

    # Vòng lặp chính
    iteration = 0
    max_iterations = 100  # Giới hạn số vòng lặp
    
    while iteration < max_iterations:
        iteration += 1
        
        # Chọn các widow tốt nhất để làm cha mẹ
        parents_indices = np.argsort(fitness_values)[:N_R]
        parents = widows[parents_indices]
        
        # Tạo ra mảng trẻ con
        children = []
        
        for i in range(N_R):
            # Chọn ngẫu nhiên 2 solutions từ các bậc cha mẹ
            parent1, parent2 = parents[np.random.choice(N_R, 2, replace=False)]
            
            # Tạo D đứa con (crossover)
            crossover_point = np.random.randint(1, D)
            child = np.concatenate((parent1[:crossover_point], parent2[crossover_point:]))
            children.append(child)
        
        # Chọn những đứa con từ cannibalism (chọn từ các solution tốt nhất)
        cannibalism_count = int(CannibalismRate * D)
        children = np.array(children)
        best_children = np.argsort(fitness_values[parents_indices])[:cannibalism_count]
        children[:cannibalism_count] = widows[best_children]

        # Đột biến
        mutated = []
        for i in range(int(N * MutationRate)):
            # Chọn ngẫu nhiên 1 đứa con để đột biến
            mutated_child = children[np.random.randint(N_R)]
            mutation = np.random.uniform(-1, 1, size=D)  # Đột biến ngẫu nhiên
            mutated_child += mutation
            mutated.append(mutated_child)
        
        # Cập nhật dân số mới (bao gồm cả trẻ con và các cá thể đột biến)
        widows = np.vstack((children, mutated))
        
        # Đánh giá lại độ phù hợp
        fitness_values = np.apply_along_axis(f, 1, widows)
        
        # Tìm lại widow tốt nhất
        current_best_solution, current_best_fitness = find_best(widows, fitness_values)
        
        # Cập nhật nếu tìm được solution tốt hơn
        if current_best_fitness < best_fitness:
            best_solution = current_best_solution
            best_fitness = current_best_fitness

    return best_solution, best_fitness

# Hàm fitness giả định cho hệ thống mạng
# Ví dụ: f(x) = c1 * băng thông + c2 * chi phí + c3 * độ trễ (tối ưu hóa băng thông và chi phí)
def fitness_function(x):
    # Giả sử:
    # x[0] là băng thông
    # x[1] là chi phí
    # x[2] là độ trễ
    
    bandwidth = x[0]
    cost = x[1]
    latency = x[2]
    
    # Hàm fitness cần tối ưu hóa: giả sử ta muốn tối đa hóa băng thông, nhưng giảm thiểu chi phí và độ trễ
    c1, c2, c3 = 1, 1, 1  # Các trọng số cho băng thông, chi phí và độ trễ
    fitness = c2 * cost + c3 * latency - c1 * bandwidth  # Fitness có thể là một hàm kết hợp

    return fitness

# Tham số thuật toán
N = 50  # Kích thước dân số
D = 3   # Số chiều của bài toán (băng thông, chi phí, độ trễ)
ReproductionRate = 0.6
CannibalismRate = 0.2
MutationRate = 0.3

# Gọi thuật toán
best_solution, best_fitness = black_widow_optimization(N, D, ReproductionRate, CannibalismRate, MutationRate, fitness_function)

# In kết quả
print("Best solution:", best_solution)
print("Best fitness:", best_fitness)
