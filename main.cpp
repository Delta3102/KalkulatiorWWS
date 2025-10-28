#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>
#include <memory>
#include <stdexcept>

// Класс для представления дробей
class Fraction {
private:
    long long numerator;
    long long denominator;

    // Нахождение НОД
    long long gcd(long long a, long long b) {
        return b == 0 ? a : gcd(b, a % b);
    }

    // Сокращение дроби
    void reduce() {
        if (denominator < 0) {
            numerator = -numerator;
            denominator = -denominator;
        }
        long long common = gcd(std::abs(numerator), std::abs(denominator));
        numerator /= common;
        denominator /= common;
    }

public:
    Fraction(long long num = 0, long long den = 1) : numerator(num), denominator(den) {
        if (denominator == 0) {
            throw std::runtime_error("Denominator cannot be zero");
        }
        reduce();
    }

    // Геттеры
    long long getNumerator() const { return numerator; }
    long long getDenominator() const { return denominator; }

    // Арифметические операции
    Fraction operator+(const Fraction& other) const {
        long long new_num = numerator * other.denominator + other.numerator * denominator;
        long long new_den = denominator * other.denominator;
        return Fraction(new_num, new_den);
    }

    Fraction operator-(const Fraction& other) const {
        long long new_num = numerator * other.denominator - other.numerator * denominator;
        long long new_den = denominator * other.denominator;
        return Fraction(new_num, new_den);
    }

    Fraction operator*(const Fraction& other) const {
        long long new_num = numerator * other.numerator;
        long long new_den = denominator * other.denominator;
        return Fraction(new_num, new_den);
    }

    Fraction operator/(const Fraction& other) const {
        if (other.numerator == 0) {
            throw std::runtime_error("Division by zero");
        }
        long long new_num = numerator * other.denominator;
        long long new_den = denominator * other.numerator;
        return Fraction(new_num, new_den);
    }

    // Возведение в степень
    Fraction pow(int exponent) const {
        if (exponent == 0) return Fraction(1, 1);
        
        Fraction result(1, 1);
        Fraction base = *this;
        int abs_exp = std::abs(exponent);
        
        for (int i = 0; i < abs_exp; i++) {
            result = result * base;
        }
        
        if (exponent < 0) {
            return Fraction(1, 1) / result;
        }
        return result;
    }

    // Преобразование в double
    double toDouble() const {
        return static_cast<double>(numerator) / denominator;
    }

    // Вывод дроби
    friend std::ostream& operator<<(std::ostream& os, const Fraction& frac) {
        if (frac.denominator == 1) {
            os << frac.numerator;
        } else {
            os << frac.numerator << "/" << frac.denominator;
        }
        return os;
    }
};

// Абстрактный класс для узлов AST
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual Fraction evaluate() const = 0;
    virtual void print(int indent = 0) const = 0;
};

// Узел для чисел
class NumberNode : public ASTNode {
private:
    Fraction value;

public:
    NumberNode(const Fraction& val) : value(val) {}
    
    Fraction evaluate() const override {
        return value;
    }
    
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Number: " << value << std::endl;
    }
};

// Узел для бинарных операций
class BinaryOpNode : public ASTNode {
private:
    char op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

public:
    BinaryOpNode(char operation, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
        : op(operation), left(std::move(l)), right(std::move(r)) {}
    
    Fraction evaluate() const override {
        Fraction left_val = left->evaluate();
        Fraction right_val = right->evaluate();
        
        switch (op) {
            case '+': return left_val + right_val;
            case '-': return left_val - right_val;
            case '*': return left_val * right_val;
            case '/': return left_val / right_val;
            case '^': return left_val.pow(static_cast<int>(right_val.toDouble()));
            default: throw std::runtime_error("Unknown operator");
        }
    }
    
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "BinaryOp: " << op << std::endl;
        left->print(indent + 2);
        right->print(indent + 2);
    }
};

// Класс парсера и лексера
class Parser {
private:
    std::string input;
    size_t pos;
    char current_char;
    
    void advance() {
        pos++;
        if (pos < input.length()) {
            current_char = input[pos];
        } else {
            current_char = '\0';
        }
    }
    
    void skip_whitespace() {
        while (current_char != '\0' && std::isspace(current_char)) {
            advance();
        }
    }
    
    // Парсинг числа (целое или дробь)
    std::unique_ptr<ASTNode> parse_number() {
        std::string number_str;
        
        // Читаем первую часть числа
        while (current_char != '\0' && (std::isdigit(current_char) || current_char == '-')) {
            number_str += current_char;
            advance();
        }
        
        // Если есть дробная часть
        if (current_char == '/') {
            advance();
            std::string denominator_str;
            while (current_char != '\0' && std::isdigit(current_char)) {
                denominator_str += current_char;
                advance();
            }
            
            long long num = std::stoll(number_str);
            long long den = std::stoll(denominator_str);
            return std::make_unique<NumberNode>(Fraction(num, den));
        } else {
            long long num = std::stoll(number_str);
            return std::make_unique<NumberNode>(Fraction(num, 1));
        }
    }
    
    std::unique_ptr<ASTNode> parse_factor() {
        skip_whitespace();
        
        if (current_char == '(') {
            advance();
            auto node = parse_expression();
            skip_whitespace();
            if (current_char == ')') {
                advance();
            } else {
                throw std::runtime_error("Expected ')'");
            }
            return node;
        } else if (std::isdigit(current_char) || current_char == '-') {
            return parse_number();
        } else {
            throw std::runtime_error("Unexpected character");
        }
    }
    
    std::unique_ptr<ASTNode> parse_term() {
        auto node = parse_factor();
        
        while (true) {
            skip_whitespace();
            if (current_char == '*' || current_char == '/') {
                char op = current_char;
                advance();
                auto right = parse_factor();
                node = std::make_unique<BinaryOpNode>(op, std::move(node), std::move(right));
            } else if (current_char == '^') {
                char op = current_char;
                advance();
                auto right = parse_factor();
                node = std::make_unique<BinaryOpNode>(op, std::move(node), std::move(right));
            } else {
                break;
            }
        }
        
        return node;
    }
    
    std::unique_ptr<ASTNode> parse_expression() {
        auto node = parse_term();
        
        while (true) {
            skip_whitespace();
            if (current_char == '+' || current_char == '-') {
                char op = current_char;
                advance();
                auto right = parse_term();
                node = std::make_unique<BinaryOpNode>(op, std::move(node), std::move(right));
            } else {
                break;
            }
        }
        
        return node;
    }

public:
    Parser(const std::string& str) : input(str), pos(0) {
        current_char = input.empty() ? '\0' : input[0];
    }
    
    std::unique_ptr<ASTNode> parse() {
        return parse_expression();
    }
};

// Класс компилятора
class Compiler {
public:
    Fraction compile_and_evaluate(const std::string& expression) {
        try {
            Parser parser(expression);
            auto ast = parser.parse();
            
            std::cout << "AST:" << std::endl;
            ast->print();
            
            Fraction result = ast->evaluate();
            return result;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            throw;
        }
    }
};

// Интерактивная оболочка
void interactive_shell() {
    Compiler compiler;
    std::string input;
    
    std::cout << "Advanced Fraction Compiler" << std::endl;
    std::cout << "Supports: +, -, *, /, ^, fractions (a/b)" << std::endl;
    std::cout << "Type 'quit' to exit" << std::endl;
    std::cout << std::string(40, '=') << std::endl;
    
    while (true) {
        std::cout << ">>> ";
        std::getline(std::cin, input);
        
        if (input == "quit" || input == "exit") {
            break;
        }
        
        if (input.empty()) {
            continue;
        }
        
        try {
            Fraction result = compiler.compile_and_evaluate(input);
            std::cout << "Result: " << result << " (≈ " << result.toDouble() << ")" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Compilation error: " << e.what() << std::endl;
        }
        
        std::cout << std::endl;
    }
}

int main() {
    // Примеры использования
    Compiler compiler;
    
    std::cout << "Примеры вычислений:" << std::endl;
    std::cout << std::string(40, '-') << std::endl;
    
    std::vector<std::string> examples = {
        "1/2 + 1/3",
        "3/4 * 2/3",
        "2/3 ^ 2",
        "1/2 + 1/4 * 2",
        "(1/2 + 1/3) * 3/4",
        "2 ^ 3",
        "4/2 - 1/3"
    };
    
    for (const auto& example : examples) {
        try {
            std::cout << example << " = ";
            Fraction result = compiler.compile_and_evaluate(example);
            std::cout << result << " (≈ " << result.toDouble() << ")" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
    
    std::cout << std::endl;
    
    // Запуск интерактивной оболочки
    interactive_shell();
    
    return 0;
}