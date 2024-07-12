NAME=ircserv
CPP= c++ 
CPPFLAGS=-Wall -Wextra -Werror -std=c++98
RM=rm -f
SERVERPATH= ./server/
EXCEPTIONPATH= ./exception/
SRC= $(SERVERPATH)Server.cpp $(SERVERPATH)Authenticator.cpp $(SERVERPATH)Client.cpp \
	 $(EXCEPTIONPATH)PasswordNotAcceptedException.cpp $(EXCEPTIONPATH)CouldNotBindServerSocketException.cpp\
	 $(EXCEPTIONPATH)CouldNotListenServerSocketException.cpp $(EXCEPTIONPATH)NewClientNotAcceptedException.cpp \
	 $(EXCEPTIONPATH)NonBlockServerSocketException.cpp $(EXCEPTIONPATH)OpenServerSocketException.cpp \
	 $(EXCEPTIONPATH)PollCheckFdsEventsException.cpp $(EXCEPTIONPATH)SocketCouldNotReuseAddrException.cpp

HEADERS= $(SERVERPATH)Server.hpp $(SERVERPATH)Authenticator.hpp $(SERVERPATH)Client.hpp \
		 $(EXCEPTIONPATH)PasswordNotAcceptedException.hpp $(EXCEPTIONPATH)CouldNotBindServerSocketException.hpp\
		 $(EXCEPTIONPATH)CouldNotListenServerSocketException.hpp $(EXCEPTIONPATH)NewClientNotAcceptedException.hpp \
		 $(EXCEPTIONPATH)NonBlockServerSocketException.hpp $(EXCEPTIONPATH)OpenServerSocketException.hpp \
		 $(EXCEPTIONPATH)PollCheckFdsEventsException.hpp $(EXCEPTIONPATH)SocketCouldNotReuseAddrException.hpp

SRC_MAIN = main.cpp

	
OBJ = $(SRC:.cpp=.o)
OBJ_MAIN = $(SRC_MAIN:.cpp=.o)


TDDPATH= ./TDD/
SRC_TDD = $(TDDPATH)TestServer.cpp
OBJ_TDD = $(SRC_TDD:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ_MAIN) $(OBJ)
		@$(CPP) $(CPPFLAGS) -o $@ $(OBJ_MAIN) $(OBJ)
		@echo "the files has ben archived successfully"

%.o: %.cpp $(HEADERS)
		@$(CPP) $(CPPFLAGS) -o $@ -c $<
		@echo "the file $@ has been created from $<"


clean:
		@$(RM) $(OBJ) $(OBJ_TDD) $(OBJ_MAIN)
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

%.o: %.cpp $(HEADERS)
		@$(CPP) $(CPPFLAGS) -o $@ -c $<
		@echo "the file $@ has been created from $<"