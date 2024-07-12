#include <memory>
#include <stdexcept>
#include <assert.h>
#include <vector>

struct MathNode {
public:

	using number_type = double;
	using ptr_type = std::shared_ptr<MathNode>;
	
	template<typename... T> static ptr_type make_ptr(T... t) {
		return std::make_shared<MathNode>(t...);
	}

	enum class Type {
		NONE,

		CONSTANT,
		VARIABLE,
		OP_PLUS,
		OP_TIMES,
	};

	const Type _type;

	static int get_operand_count(Type type) {
		switch (type) {
			case Type::CONSTANT:
			case Type::VARIABLE:
				return 0;
			case Type::OP_PLUS:
			case Type::OP_TIMES:
				return 2;
		}

		throw std::runtime_error("unreachable");
	}

private:

	union {
		const number_type _value;
		const char *_name;
		const ptr_type _op[2];
	};

public:

	MathNode(number_type value) : _type(Type::CONSTANT), _value(value) {}
	MathNode(const char *name) : _type(Type::VARIABLE), _name(name) {}
	MathNode(Type type, ptr_type a, ptr_type b) : _type(type), _op{ a, b } { assert(get_operand_count(type) == 2); }

	~MathNode() {
		const int operand_count = get_operand_count(_type);
		for (int i = 0; i < operand_count; i++) {
			_op[i].~shared_ptr();
		}
	}

	static ptr_type constant(number_type value) {
		return make_ptr(value);
	}

	static ptr_type variable(const char *name) {
		throw std::runtime_error("unimplemented");
	}

	static ptr_type eq(ptr_type a, ptr_type b) {
		throw std::runtime_error("unimplemented");
	}

	// ptr_type simplest

	std::string to_string() {
		switch(_type) {
			case Type::CONSTANT: return std::to_string(_value);
			case Type::VARIABLE: return std::string(_name);
			case Type::OP_PLUS: return _op[0]->to_string() + std::string("+") + _op[1]->to_string();
			case Type::OP_TIMES: return _op[0]->to_string() + std::string("*") + _op[1]->to_string();
		}

		throw std::runtime_error("unreachable");
	}

	static Type *operand_type_map;

	struct Token {
		enum class Type {
			NUMBER,
			OPERAND
		};

		Type type;
		union {
			number_type value;
			MathNode::Type operand;
		};

		Token(number_type value) : type(Type::NUMBER), value(value) {}
		Token(MathNode::Type operand) : type(Type::OPERAND), operand(operand) {}

		static char operand_to_char(MathNode::Type type) {
			switch (type) {
				case MathNode::Type::OP_PLUS: return '+';
				case MathNode::Type::OP_TIMES: return '*';
			}

			throw std::runtime_error("unreachable");
		}
	};

	static std::vector<Token> tokenize(const std::string &str) {
		std::vector<Token> tokens;

		auto length = str.length();
		for (int i = 0; i < length; i++) {
			if (isspace(str[i])) {
				++i;
				continue;
			}

			if (isdigit(str[i]) || str[i] == '.') {
				char *end;
				tokens.emplace_back(strtod(&str[i], &end));
				i += end - &str[i] - 1;
			} else if (Type type = operand_type_map[str[i]]; type != Type::NONE) {
				tokens.emplace_back(type);
			} else {
				throw std::runtime_error("invalid input: " + str);
			}
		}

		return tokens;
	}

	static ptr_type parse(const std::string &str) {
		std::vector<Token> tokens = tokenize(str);
		
		for (auto &token : tokens) {
			switch (token.type) {
				case Token::Type::NUMBER: {
					printf("\t%s: %lf\n", "NUMBER", token.value);
					break;
				}
				case Token::Type::OPERAND: {
					printf("\t%s: %c\n", "OPERAND", Token::operand_to_char(token.operand));
					break;
				}
				default: throw std::runtime_error("unimplemented");
			}
		}

		return make_ptr(0);
	}

};

MathNode::Type *MathNode::operand_type_map = ([]() {
	static MathNode::Type operand_type_map[256] = { };
	operand_type_map['+'] = MathNode::Type::OP_PLUS;
	operand_type_map['*'] = MathNode::Type::OP_TIMES;
	return operand_type_map;
})();

#include <stdio.h>

int main(int argc, const char **argv) {
	auto input = MathNode::parse(argv[1]);

	std::string result;
	try {
		// result = input.to_string();
	} catch (std::exception &e) {
		result = "error";
	}

	printf("%s\n", result.c_str());
}
