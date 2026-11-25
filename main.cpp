#include <iostream>
#include <vector>
#include <ctime>
#include <string>
#include <fstream>

class Employee{
public:
    std::string name;
    float salary;
    Employee(const std::string& n, float s)
        : name(n), salary(s) {}
};

class Customer{
public:
    std::string name;
    std::string address;
    Customer(const std::string& n, const std::string& a)
        : name(n), address(a) {}
};

class Sale{
public:
    Customer* customer;      
    Employee* salerep;      
    float amount;
    std::time_t date;
    Sale(Customer* c, Employee* e, float amt)
        : customer(c), salerep(e), amount(amt) {
        date = std::time(nullptr); 
    }
};


void addEmployee(std::vector<Employee>& employees) {
    std::string name;
    float salary;

    std::cout << "enter employee name: ";
    std::getline(std::cin >> std::ws, name);  
    std::cout << "enter employee salary: ";
    std::cin >> salary;

    employees.emplace_back(name, salary);
    std::cout << "employee added successfully\n";
}

void addCustomer(std::vector<Customer>& customers) {
    std::string name, address;

    std::cout << "enter customer name: ";
    std::getline(std::cin >> std::ws, name);
    std::cout << "enter customer address: ";
    std::getline(std::cin >> std::ws, address);

    customers.emplace_back(name, address);
    std::cout << "customer added successfully\n";
}

void addSale(std::vector<Sale>& sales,
             std::vector<Customer>& customers,
             std::vector<Employee>& employees) {
    if (customers.empty() || employees.empty()) {
        std::cout << "add at least one customer and one employee\n";
        return;
    }

    std::cout << "select customer:\n";
    for (size_t i = 0; i < customers.size(); ++i) {
        std::cout << i << ". " << customers[i].name << "\n";
    }
    int custIndex;
    std::cout << "enter index: ";
    std::cin >> custIndex;

    if (custIndex < 0 || custIndex >= customers.size()) {
        std::cout << "invalid index\n";
        return;
    }

    std::cout << "select sales representative:\n";
    for (size_t i = 0; i < employees.size(); ++i) {
        std::cout << i << ". " << employees[i].name << "\n";
    }
    int empIndex;
    std::cout << "enter index: ";
    std::cin >> empIndex;

    if (empIndex < 0 || empIndex >= employees.size()) {
        std::cout << "invalid index\n";
        return;
    }

    float amount;
    std::cout << "enter sale amount: ";
    std::cin >> amount;

    // Create and store the sale
    sales.emplace_back(&customers[custIndex], &employees[empIndex], amount);
    std::cout << "sale added successfully\n";
}

void printEmployees(const std::vector<Employee>& employees) {
    std::cout << "\n--- employees ---\n";
    for (const auto& emp : employees) {
        std::cout << "name: " << emp.name << ", salary: " << emp.salary << "\n";
    }
}

void printCustomers(const std::vector<Customer>& customers) {
    std::cout << "\n--- customers ---\n";
    for (const auto& cust : customers) {
        std::cout << "name: " << cust.name << ", address: " << cust.address << "\n";
    }
}

void printSales(const std::vector<Sale>& sales) {
    std::cout << "\n--- sales ---\n";
    for (const auto& sale : sales) {
        std::cout << "customer: " << sale.customer->name
                  << ", sales rep: " << sale.salerep->name
                  << ", amount: " << sale.amount
                  << ", date: " << std::ctime(&sale.date); 
    }
}

void writeFile(const std::vector<Employee>& employees,
               const std::vector<Customer>& customers,
               const std::vector<Sale>& sales) {
    std::ofstream out("Data.bin", std::ios::binary);
    if (!out) {
        std::cerr << "error opening file\n";
        return;
    }

    // Write Employees
    size_t empCount = employees.size();
    out.write(reinterpret_cast<char*>(&empCount), sizeof(empCount));
    for (const auto& emp : employees) {
        size_t nameLen = emp.name.size();
        out.write(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        out.write(emp.name.c_str(), nameLen);
        out.write(reinterpret_cast<const char*>(&emp.salary), sizeof(emp.salary));
    }

    // Write Customers
    size_t custCount = customers.size();
    out.write(reinterpret_cast<char*>(&custCount), sizeof(custCount));
    for (const auto& cust : customers) {
        size_t nameLen = cust.name.size();
        size_t addrLen = cust.address.size();
        out.write(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        out.write(cust.name.c_str(), nameLen);
        out.write(reinterpret_cast<char*>(&addrLen), sizeof(addrLen));
        out.write(cust.address.c_str(), addrLen);
    }

    // Write Sales
    size_t saleCount = sales.size();
    out.write(reinterpret_cast<char*>(&saleCount), sizeof(saleCount));
    for (const auto& sale : sales) {
        // store index instead of full object
        int custIndex = -1;
        int empIndex = -1;

        // find pointer index in original lists
        for (size_t i = 0; i < customers.size(); ++i) {
            if (&customers[i] == sale.customer) {
                custIndex = i;
                break;
            }
        }
        for (size_t i = 0; i < employees.size(); ++i) {
            if (&employees[i] == sale.salerep) {
                empIndex = i;
                break;
            }
        }

        out.write(reinterpret_cast<char*>(&custIndex), sizeof(custIndex));
        out.write(reinterpret_cast<char*>(&empIndex), sizeof(empIndex));
        out.write(reinterpret_cast<const char*>(&sale.amount), sizeof(sale.amount));
        out.write(reinterpret_cast<const char*>(&sale.date), sizeof(sale.date));
    }

    out.close();
    std::cout << "data saveed successfully to Data.bin\n";
}

void loadFile(std::vector<Employee>& employees,
              std::vector<Customer>& customers,
              std::vector<Sale>& sales) {
    std::ifstream in("Data.bin", std::ios::binary);
    if (!in) {
        std::cout << "No saved data found (Data.bin missing).\n";
        return;
    }

    // --- Load Employees ---
    size_t empCount;
    in.read(reinterpret_cast<char*>(&empCount), sizeof(empCount));
    employees.clear();
    for (size_t i = 0; i < empCount; ++i) {
        size_t nameLen;
        in.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        std::string name(nameLen, ' ');
        in.read(&name[0], nameLen);

        float salary;
        in.read(reinterpret_cast<char*>(&salary), sizeof(salary));

        employees.push_back(Employee{name, salary});
    }

    // --- Load Customers ---
    size_t custCount;
    in.read(reinterpret_cast<char*>(&custCount), sizeof(custCount));
    customers.clear();
    for (size_t i = 0; i < custCount; ++i) {
        size_t nameLen, addrLen;
        in.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        std::string name(nameLen, ' ');
        in.read(&name[0], nameLen);

        in.read(reinterpret_cast<char*>(&addrLen), sizeof(addrLen));
        std::string addr(addrLen, ' ');
        in.read(&addr[0], addrLen);

        customers.push_back(Customer{name, addr});
    }

    // --- Load Sales ---
    size_t saleCount;
    in.read(reinterpret_cast<char*>(&saleCount), sizeof(saleCount));
    sales.clear();
    for (size_t i = 0; i < saleCount; ++i) {
        int custIndex, empIndex;
        float amount;
        std::time_t date;

        in.read(reinterpret_cast<char*>(&custIndex), sizeof(custIndex));
        in.read(reinterpret_cast<char*>(&empIndex), sizeof(empIndex));
        in.read(reinterpret_cast<char*>(&amount), sizeof(amount));
        in.read(reinterpret_cast<char*>(&date), sizeof(date));

        if (custIndex >= 0 && custIndex < customers.size() &&
            empIndex >= 0 && empIndex < employees.size()) {
            sales.emplace_back(&customers[custIndex], &employees[empIndex], amount);
            sales.back().date = date; // set correct timestamp
        }
    }

    in.close();
    std::cout << "Data loaded successfully from Data.bin\n";
}



int main() {
    std::cout << "Sales Management System" << std::endl;
    std::vector<Employee> employees;
    std::vector<Customer> customers;
    std::vector<Sale> sales;

    loadFile(employees, customers, sales);

    while (true) {
        std::cout << "\n--- menu ---\n";
        std::cout << "1. add employee\n";
        std::cout << "2. add customer\n";
        std::cout << "3. add sale\n";
        std::cout << "4. show all data\n";
        std::cout << "5. exit\n";
        std::cout << "enter your choice: ";

        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1:
                addEmployee(employees);
                break;
            case 2:
                addCustomer(customers);
                break;
            case 3:
                addSale(sales, customers, employees);
                break;
            case 4:
                printEmployees(employees);
                printCustomers(customers);
                printSales(sales);
                break;
            case 5:
                writeFile(employees, customers, sales);
                std::cout << "exiting program\n";
                return 0;
            default:
                std::cout << "invalid choice, try again\n";
        }
    }
    return 0;
}
