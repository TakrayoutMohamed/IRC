NAME = main

COMPILER = c++

FLAGS = -Wall -Wextra -Werror -std=c++98

CPPFILES = 	main.cpp \
			Commands.cpp


OBJCPP = $(CPPFILES:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJCPP)
	$(COMPILER) $(FLAGS) $(OBJCPP) -o $(NAME)

%.o: %.cpp Client.hpp Commands.hpp
	$(COMPILER) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJCPP)

fclean: clean
	rm -rf $(NAME)

re: fclean all