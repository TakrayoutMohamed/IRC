NAME = main

COMPILER = c++

FLAGS = -Wall -Wextra -Werror -std=c++98

CPPFILES = 	main.cpp \
			Commands.cpp


OBJCPP = $(CPPFILES:.cpp=.o)

all: $(NAME)

test: 
	$(COMPILER) $(FLAGS) tets.cpp -o testing_mode
$(NAME): $(OBJCPP)
	$(COMPILER) $(FLAGS) $(OBJCPP) -o $(NAME)

%.o: %.cpp Client.hpp Commands.hpp
	$(COMPILER) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJCPP)

fclean: clean
	rm -rf $(NAME)

re: fclean all

# i > invite
# k > key
# l > limit
# o > operator
# t > topic

# i = +i > set invite
# -i > remove invite

# i and t doesn't need parameters
# same goes for -l

# -kkkkkkkkkkkk+lllkkkkkkk

# mode #ch1 