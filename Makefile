NAME=ircserv
CPP= c++ 
CPPFLAGS=-Wall -Wextra -Werror -std=c++98
RM=rm -f
SERVERPATH= ./server/
EXCEPTIONPATH= ./exception/
SRC= $(SERVERPATH)Server.cpp \
	 $(EXCEPTIONPATH)PasswordNotAcceptedException.cpp \
	 $(SERVERPATH)Authenticator.cpp
SRC_MAIN = main.cpp

	
OBJ = $(SRC:.cpp=.o)
OBJ_MAIN = $(SRC_MAIN:.cpp=.o)


TDDPATH= ./TDD/
SRC_TDD = $(TDDPATH)TestServer.cpp
OBJ_TDD = $(SRC_TDD:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
		@$(CPP) $(CPPFLAGS) -o $@ $(OBJ)
		@echo "the files has ben archived successfully"

%.o: %.cpp $(SERVERPATH)Server.hpp $(EXCEPTIONPATH)PasswordNotAcceptedException.hpp
		@$(CPP) $(CPPFLAGS) -o $@ -c $<
		@echo "the file $@ has been created from $<"


clean:
		@$(RM) $(OBJ) $(OBJ_TDD)
		@echo "all the .o has been deleted successfully"
fclean: clean
		@$(RM) $(NAME)
		@echo "the $(NAME) has been deleted"
	
re: fclean all

.PHONY : clean $(NAME) all fclean re
# /////////////////////////////////
test:$(OBJ_TDD) $(OBJ) 
		@$(CPP) $(CPPFLAGS) -o $@ $(OBJ_TDD) $(OBJ) 
		@echo "the files has ben archived successfully"

%.o: %.cpp $(EXCEPTIONPATH)PasswordNotAcceptedException.hpp $(SERVERPATH)Server.hpp $(SERVERPATH)Authenticator.hpp
		@$(CPP) $(CPPFLAGS) -o $@ -c $<
		@echo "the file $@ has been created from $<"