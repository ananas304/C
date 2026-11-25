#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <limits>
#include <cstdint>

class Employee
{
public:
    int id;
    std::string name;
    double salary;

    Employee() : id(0), salary(0.0) {}
    Employee(int i, const std::string &n, double s)
        : id(i), name(n), salary(s) {}

    int getId() const { return id; }
};

class Customer
{
public:
    int id;
    std::string name;
    std::string address;

    Customer() : id(0) {}
    Customer(int i, const std::string &n, const std::string &a)
        : id(i), name(n), address(a) {}

    int getId() const { return id; }
};

class Sale
{
public:
    int id;
    int customerId;
    int employeeId;
    double amount;
    std::time_t date;

    Sale() : id(0), customerId(0), employeeId(0), amount(0.0), date(0) {}
    Sale(int i, int cId, int eId, double a)
        : id(i), customerId(cId), employeeId(eId), amount(a),
          date(std::time(nullptr)) {}

    int getId() const { return id; }
};

template <typename T>
class BinarySearchTree
{
private:
    struct Node
    {
        T data;
        Node *left;
        Node *right;

        explicit Node(const T &value)
            : data(value), left(nullptr), right(nullptr) {}
    };

    Node *root;
    std::size_t nodeCount;

    Node *insertNode(Node *node, const T &value, bool &inserted)
    {
        if (node == nullptr)
        {
            inserted = true;
            return new Node(value);
        }

        if (value.getId() < node->data.getId())
        {
            node->left = insertNode(node->left, value, inserted);
        }
        else if (value.getId() > node->data.getId())
        {
            node->right = insertNode(node->right, value, inserted);
        }
        return node;
    }

    Node *findNode(Node *node, int id) const
    {
        if (node == nullptr || node->data.getId() == id)
        {
            return node;
        }
        if (id < node->data.getId())
        {
            return findNode(node->left, id);
        }
        return findNode(node->right, id);
    }

    Node *findMin(Node *node) const
    {
        while (node != nullptr && node->left != nullptr)
        {
            node = node->left;
        }
        return node;
    }

    Node *eraseNode(Node *node, int id, bool &erased)
    {
        if (node == nullptr)
        {
            return nullptr;
        }

        if (id < node->data.getId())
        {
            node->left = eraseNode(node->left, id, erased);
        }
        else if (id > node->data.getId())
        {
            node->right = eraseNode(node->right, id, erased);
        }
        else
        {
            erased = true;

            // Case 1: no child or only a right child.
            if (node->left == nullptr)
            {
                Node *replacement = node->right;
                delete node;
                return replacement;
            }

            // Case 2: only a left child.
            if (node->right == nullptr)
            {
                Node *replacement = node->left;
                delete node;
                return replacement;
            }

            // Case 3: two children. Copy the inorder successor, then remove the successor from the right subtree.
            Node *successor = findMin(node->right);
            node->data = successor->data;
            bool successorErased = false;
            node->right = eraseNode(node->right, successor->data.getId(), successorErased);
        }
        return node;
    }

    template <typename Visitor>
    void inOrderNode(Node *node, Visitor &visit) const
    {
        if (node == nullptr)
            return;
        inOrderNode(node->left, visit);
        visit(node->data);
        inOrderNode(node->right, visit);
    }

    template <typename Visitor>
    void preOrderNode(Node *node, Visitor &visit) const
    {
        if (node == nullptr)
            return;
        visit(node->data);
        preOrderNode(node->left, visit);
        preOrderNode(node->right, visit);
    }

    template <typename Visitor>
    void postOrderNode(Node *node, Visitor &visit) const
    {
        if (node == nullptr)
            return;
        postOrderNode(node->left, visit);
        postOrderNode(node->right, visit);
        visit(node->data);
    }

    void clearNode(Node *node)
    {
        if (node == nullptr)
            return;
        clearNode(node->left);
        clearNode(node->right);
        delete node;
    }

public:
    BinarySearchTree() : root(nullptr), nodeCount(0) {}

    ~BinarySearchTree()
    {
        clearNode(root);
    }

    // Prevent accidental shallow copies of the tree's owned nodes.
    BinarySearchTree(const BinarySearchTree &) = delete;
    BinarySearchTree &operator=(const BinarySearchTree &) = delete;

    bool insert(const T &value)
    {
        bool inserted = false;
        root = insertNode(root, value, inserted);
        if (inserted)
            ++nodeCount;
        return inserted;
    }

    T *search(int id)
    {
        Node *result = findNode(root, id);
        return result == nullptr ? nullptr : &result->data;
    }

    const T *search(int id) const
    {
        Node *result = findNode(root, id);
        return result == nullptr ? nullptr : &result->data;
    }

    bool remove(int id)
    {
        bool erased = false;
        root = eraseNode(root, id, erased);
        if (erased)
            --nodeCount;
        return erased;
    }

    std::size_t size() const { return nodeCount; }
    bool empty() const { return root == nullptr; }

    void clear()
    {
        clearNode(root);
        root = nullptr;
        nodeCount = 0;
    }

    template <typename Visitor>
    void inOrder(Visitor visit) const
    {
        inOrderNode(root, visit);
    }

    template <typename Visitor>
    void preOrder(Visitor visit) const
    {
        preOrderNode(root, visit);
    }

    template <typename Visitor>
    void postOrder(Visitor visit) const
    {
        postOrderNode(root, visit);
    }
};

void printEmployee(const Employee &employee)
{
    std::cout << "ID: " << employee.id
              << " | Name: " << employee.name
              << " | Salary: " << employee.salary << '\n';
}

void printCustomer(const Customer &customer)
{
    std::cout << "ID: " << customer.id
              << " | Name: " << customer.name
              << " | Address: " << customer.address << '\n';
}

void printSale(const Sale &sale,
               const BinarySearchTree<Customer> &customers,
               const BinarySearchTree<Employee> &employees)
{
    const Customer *customer = customers.search(sale.customerId);
    const Employee *employee = employees.search(sale.employeeId);

    std::cout << "Sale ID: " << sale.id
              << " | Customer: " << (customer ? customer->name : "[not found]")
              << " | Sales representative: " << (employee ? employee->name : "[not found]")
              << " | Amount: " << sale.amount
              << " | Date: ";

    std::tm *timeInfo = std::localtime(&sale.date);
    if (timeInfo != nullptr)
    {
        char buffer[30];
        if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo) > 0)
            std::cout << buffer;
        else
            std::cout << "unknown";
    }
    else
    {
        std::cout << "unknown";
    }
    std::cout << '\n';
}

int readInt(const std::string &prompt)
{
    int value;
    while (true)
    {
        std::cout << prompt;
        if (std::cin >> value)
            return value;
        std::cout << "Please enter a valid whole number.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

double readPositiveAmount(const std::string &prompt, bool allowZero = false)
{
    double value;
    while (true)
    {
        std::cout << prompt;
        if (std::cin >> value && (allowZero ? value >= 0.0 : value > 0.0))
        {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cout << "Please enter a valid " << (allowZero ? "non-negative" : "positive")
                  << " amount.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::string readLine(const std::string &prompt)
{
    std::string value;
    std::cout << prompt;
    std::getline(std::cin >> std::ws, value);
    return value;
}

void addEmployee(BinarySearchTree<Employee> &employees, int &nextEmployeeId)
{
    std::string name = readLine("Enter employee name: ");
    double salary = readPositiveAmount("Enter employee salary: ", true);

    Employee employee(nextEmployeeId, name, salary);
    if (employees.insert(employee))
    {
        std::cout << "Employee added. Assigned ID: " << nextEmployeeId << '\n';
        ++nextEmployeeId;
    }
}

void addCustomer(BinarySearchTree<Customer> &customers, int &nextCustomerId)
{
    std::string name = readLine("Enter customer name: ");
    std::string address = readLine("Enter customer address: ");

    Customer customer(nextCustomerId, name, address);
    if (customers.insert(customer))
    {
        std::cout << "Customer added. Assigned ID: " << nextCustomerId << '\n';
        ++nextCustomerId;
    }
}

void addSale(BinarySearchTree<Sale> &sales,
             const BinarySearchTree<Customer> &customers,
             const BinarySearchTree<Employee> &employees,
             int &nextSaleId)
{
    if (customers.empty() || employees.empty())
    {
        std::cout << "Add at least one customer and one employee before recording a sale.\n";
        return;
    }

    std::cout << "\nCustomers:\n";
    customers.inOrder([](const Customer &customer)
                      { printCustomer(customer); });
    int customerId = readInt("Enter customer ID: ");
    if (customers.search(customerId) == nullptr)
    {
        std::cout << "Customer ID not found. Sale was not added.\n";
        return;
    }

    std::cout << "\nEmployees:\n";
    employees.inOrder([](const Employee &employee)
                      { printEmployee(employee); });
    int employeeId = readInt("Enter sales representative ID: ");
    if (employees.search(employeeId) == nullptr)
    {
        std::cout << "Employee ID not found. Sale was not added.\n";
        return;
    }

    double amount = readPositiveAmount("Enter sale amount: ");
    Sale sale(nextSaleId, customerId, employeeId, amount);
    if (sales.insert(sale))
    {
        std::cout << "Sale added. Assigned ID: " << nextSaleId << '\n';
        ++nextSaleId;
    }
}

void displayAll(const BinarySearchTree<Employee> &employees,
                const BinarySearchTree<Customer> &customers,
                const BinarySearchTree<Sale> &sales)
{
    std::cout << "\n========== EMPLOYEES (inorder by ID) ==========\n";
    if (employees.empty())
        std::cout << "No employees found.\n";
    employees.inOrder([](const Employee &employee)
                      { printEmployee(employee); });

    std::cout << "\n========== CUSTOMERS (inorder by ID) ==========\n";
    if (customers.empty())
        std::cout << "No customers found.\n";
    customers.inOrder([](const Customer &customer)
                      { printCustomer(customer); });

    std::cout << "\n========== SALES (inorder by ID) ==========\n";
    if (sales.empty())
        std::cout << "No sales found.\n";
    sales.inOrder([&](const Sale &sale)
                  { printSale(sale, customers, employees); });
}

void searchRecord(const BinarySearchTree<Employee> &employees,
                  const BinarySearchTree<Customer> &customers,
                  const BinarySearchTree<Sale> &sales)
{
    std::cout << "1. Employee\n2. Customer\n3. Sale\n";
    int type = readInt("Choose record type: ");
    int id = readInt("Enter record ID: ");

    if (type == 1)
    {
        const Employee *result = employees.search(id);
        if (result)
            printEmployee(*result);
        else
            std::cout << "Employee not found.\n";
    }
    else if (type == 2)
    {
        const Customer *result = customers.search(id);
        if (result)
            printCustomer(*result);
        else
            std::cout << "Customer not found.\n";
    }
    else if (type == 3)
    {
        const Sale *result = sales.search(id);
        if (result)
            printSale(*result, customers, employees);
        else
            std::cout << "Sale not found.\n";
    }
    else
    {
        std::cout << "Invalid record type.\n";
    }
}

bool saleUsesEmployee(const BinarySearchTree<Sale> &sales, int employeeId)
{
    bool used = false;
    sales.inOrder([&](const Sale &sale)
                  {
        if (sale.employeeId == employeeId) used = true; });
    return used;
}

bool saleUsesCustomer(const BinarySearchTree<Sale> &sales, int customerId)
{
    bool used = false;
    sales.inOrder([&](const Sale &sale)
                  {
        if (sale.customerId == customerId) used = true; });
    return used;
}

void deleteRecord(BinarySearchTree<Employee> &employees,
                  BinarySearchTree<Customer> &customers,
                  BinarySearchTree<Sale> &sales)
{
    std::cout << "1. Employee\n2. Customer\n3. Sale\n";
    int type = readInt("Choose record type to delete: ");
    int id = readInt("Enter record ID: ");

    if (type == 1)
    {
        if (employees.search(id) == nullptr)
        {
            std::cout << "Employee not found.\n";
        }
        else if (saleUsesEmployee(sales, id))
        {
            std::cout << "Cannot delete this employee: existing sales refer to this employee.\n";
        }
        else
        {
            employees.remove(id);
            std::cout << "Employee deleted.\n";
        }
    }
    else if (type == 2)
    {
        if (customers.search(id) == nullptr)
        {
            std::cout << "Customer not found.\n";
        }
        else if (saleUsesCustomer(sales, id))
        {
            std::cout << "Cannot delete this customer: existing sales refer to this customer.\n";
        }
        else
        {
            customers.remove(id);
            std::cout << "Customer deleted.\n";
        }
    }
    else if (type == 3)
    {
        if (sales.remove(id))
            std::cout << "Sale deleted.\n";
        else
            std::cout << "Sale not found.\n";
    }
    else
    {
        std::cout << "Invalid record type.\n";
    }
}

bool writeString(std::ofstream &out, const std::string &value)
{
    std::uint32_t length = static_cast<std::uint32_t>(value.size());
    out.write(reinterpret_cast<const char *>(&length), sizeof(length));
    out.write(value.data(), static_cast<std::streamsize>(length));
    return static_cast<bool>(out);
}

bool readString(std::ifstream &in, std::string &value)
{
    std::uint32_t length = 0;
    in.read(reinterpret_cast<char *>(&length), sizeof(length));
    if (!in || length > 1'000'000)
        return false;
    value.assign(length, '\0');
    if (length > 0)
        in.read(&value[0], static_cast<std::streamsize>(length));
    return static_cast<bool>(in);
}

template <typename T>
bool writeValue(std::ofstream &out, const T &value)
{
    out.write(reinterpret_cast<const char *>(&value), sizeof(value));
    return static_cast<bool>(out);
}

template <typename T>
bool readValue(std::ifstream &in, T &value)
{
    in.read(reinterpret_cast<char *>(&value), sizeof(value));
    return static_cast<bool>(in);
}

void saveData(const BinarySearchTree<Employee> &employees,
              const BinarySearchTree<Customer> &customers,
              const BinarySearchTree<Sale> &sales,
              int nextEmployeeId, int nextCustomerId, int nextSaleId)
{
    std::ofstream out("Data.bin", std::ios::binary | std::ios::trunc);
    if (!out)
    {
        std::cerr << "Could not open Data.bin for writing.\n";
        return;
    }

    const char magic[8] = {'S', 'M', 'S', 'B', 'S', 'T', '0', '1'};
    out.write(magic, sizeof(magic));
    writeValue(out, nextEmployeeId);
    writeValue(out, nextCustomerId);
    writeValue(out, nextSaleId);

    std::uint32_t employeeCount = static_cast<std::uint32_t>(employees.size());
    writeValue(out, employeeCount);
    employees.inOrder([&](const Employee &e)
                      {
        writeValue(out, e.id);
        writeString(out, e.name);
        writeValue(out, e.salary); });

    std::uint32_t customerCount = static_cast<std::uint32_t>(customers.size());
    writeValue(out, customerCount);
    customers.inOrder([&](const Customer &c)
                      {
        writeValue(out, c.id);
        writeString(out, c.name);
        writeString(out, c.address); });

    std::uint32_t saleCount = static_cast<std::uint32_t>(sales.size());
    writeValue(out, saleCount);
    sales.inOrder([&](const Sale &s)
                  {
        writeValue(out, s.id);
        writeValue(out, s.customerId);
        writeValue(out, s.employeeId);
        writeValue(out, s.amount);
        writeValue(out, s.date); });

    if (out)
        std::cout << "Data saved successfully to Data.bin.\n";
    else
        std::cerr << "An error occurred while saving Data.bin.\n";
}

bool loadData(BinarySearchTree<Employee> &employees,
              BinarySearchTree<Customer> &customers,
              BinarySearchTree<Sale> &sales,
              int &nextEmployeeId, int &nextCustomerId, int &nextSaleId)
{
    std::ifstream in("Data.bin", std::ios::binary);
    if (!in)
    {
        std::cout << "No saved data found. Starting with empty trees.\n";
        return false;
    }

    char magic[8]{};
    const char expected[8] = {'S', 'M', 'S', 'B', 'S', 'T', '0', '1'};
    in.read(magic, sizeof(magic));
    if (!in || std::string(magic, sizeof(magic)) != std::string(expected, sizeof(expected)))
    {
        std::cout << "Data.bin has an unknown format. Starting with empty trees.\n";
        return false;
    }

    int loadedNextEmployeeId, loadedNextCustomerId, loadedNextSaleId;
    std::uint32_t count = 0;
    if (!readValue(in, loadedNextEmployeeId) ||
        !readValue(in, loadedNextCustomerId) ||
        !readValue(in, loadedNextSaleId) ||
        !readValue(in, count) || count > 1'000'000)
    {
        std::cout << "Saved data is invalid or incomplete.\n";
        return false;
    }

    employees.clear();
    customers.clear();
    sales.clear();

    for (std::uint32_t i = 0; i < count; ++i)
    {
        int id;
        double salary;
        std::string name;
        if (!readValue(in, id) || !readString(in, name) || !readValue(in, salary) ||
            !employees.insert(Employee(id, name, salary)))
        {
            std::cout << "Could not load employee records: file is invalid.\n";
            employees.clear();
            customers.clear();
            sales.clear();
            return false;
        }
    }

    if (!readValue(in, count) || count > 1'000'000)
    {
        std::cout << "Saved customer data is invalid.\n";
        employees.clear();
        return false;
    }
    for (std::uint32_t i = 0; i < count; ++i)
    {
        int id;
        std::string name, address;
        if (!readValue(in, id) || !readString(in, name) || !readString(in, address) ||
            !customers.insert(Customer(id, name, address)))
        {
            std::cout << "Could not load customer records: file is invalid.\n";
            employees.clear();
            customers.clear();
            sales.clear();
            return false;
        }
    }

    if (!readValue(in, count) || count > 1'000'000)
    {
        std::cout << "Saved sales data is invalid.\n";
        employees.clear();
        customers.clear();
        return false;
    }
    for (std::uint32_t i = 0; i < count; ++i)
    {
        int id, customerId, employeeId;
        double amount;
        std::time_t date;
        if (!readValue(in, id) || !readValue(in, customerId) ||
            !readValue(in, employeeId) || !readValue(in, amount) ||
            !readValue(in, date) || customers.search(customerId) == nullptr ||
            employees.search(employeeId) == nullptr)
        {
            std::cout << "Could not load sales records: file is invalid.\n";
            employees.clear();
            customers.clear();
            sales.clear();
            return false;
        }
        Sale sale;
        sale.id = id;
        sale.customerId = customerId;
        sale.employeeId = employeeId;
        sale.amount = amount;
        sale.date = date;
        if (!sales.insert(sale))
        {
            std::cout << "Duplicate sale ID found in saved data.\n";
            employees.clear();
            customers.clear();
            sales.clear();
            return false;
        }
    }

    nextEmployeeId = loadedNextEmployeeId;
    nextCustomerId = loadedNextCustomerId;
    nextSaleId = loadedNextSaleId;
    std::cout << "Data loaded successfully from Data.bin.\n";
    return true;
}

int main()
{
    BinarySearchTree<Employee> employees;
    BinarySearchTree<Customer> customers;
    BinarySearchTree<Sale> sales;

    int nextEmployeeId = 1;
    int nextCustomerId = 1;
    int nextSaleId = 1;

    std::cout << "========== SALES MANAGEMENT SYSTEM ==========\n";
    loadData(employees, customers, sales,
             nextEmployeeId, nextCustomerId, nextSaleId);

    while (true)
    {
        std::cout << "\n--------------- MENU ---------------\n"
                  << "1. Add employee\n"
                  << "2. Add customer\n"
                  << "3. Add sale\n"
                  << "4. Display all records\n"
                  << "5. Search record\n"
                  << "6. Delete record\n"
                  << "7. Save and exit\n";

        int choice = readInt("Enter your choice: ");

        switch (choice)
        {
        case 1:
            addEmployee(employees, nextEmployeeId);
            break;
        case 2:
            addCustomer(customers, nextCustomerId);
            break;
        case 3:
            addSale(sales, customers, employees, nextSaleId);
            break;
        case 4:
            displayAll(employees, customers, sales);
            break;
        case 5:
            searchRecord(employees, customers, sales);
            break;
        case 6:
            deleteRecord(employees, customers, sales);
            break;
        case 7:
            saveData(employees, customers, sales,
                     nextEmployeeId, nextCustomerId, nextSaleId);
            std::cout << "Exiting program.\n";
            return 0;
        default:
            std::cout << "Invalid choice. Please try again.\n";
        }
    }
}
