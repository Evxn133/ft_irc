SRCS = src/main.cpp src/server.cpp src/user.cpp

HEADERS = #server.hpp user.hpp

OBJS =	${SRCS:.cpp=.o}

NAME = ircserv

CC = c++
FLAGS = #-Wall -Wextra -Werror -std=c++98 -pedantic

.cpp.o:
	@$(call print_progress)
	@$(CC) $(FLAGS) -c $< -o ${<:.cpp=.o}
	@$(eval CURR_FILE = $(shell echo $$(($(CURR_FILE) + 1))))

all: $(NAME)

$(NAME): $(OBJS) $(HEADERS) Makefile
	@echo "Compiling: [███████████████████] 100% ($(TOTAL_FILES)/$(TOTAL_FILES))"
	@$(CC) -o $(NAME) $(OBJS) $(FLAGS)

clean:
	@rm -f *.o

fclean: clean
	@rm -f $(NAME)

re: fclean all

TOTAL_FILES := $(words $(SRCS))
CURR_FILE = 1

define print_progress
	@progress=$$((($(CURR_FILE) * 100) / $(TOTAL_FILES))); \
	printf "Compiling: ["; \
	completed=$$((progress / 5)); \
	for i in $$(seq 1 $$completed); do \
		printf "█"; \
	done; \
	for i in $$(seq $$((completed + 1)) 20); do \
		printf " "; \
	done; \
	printf "] %3d%% ($(CURR_FILE)/$(TOTAL_FILES))" $$progress
	@echo "\033[A" 
endef

git: git-add git-commit git-push

git-add:
	@git add .

git-commit:
	@echo "Entrez votre message de commit : "
	@read MESSAGE; \
	git commit -m "$$MESSAGE"

git-push:
	@git push git@github.com:evxn13/ft_irc.git

.PHONY: all clean fclean re git