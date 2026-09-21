# Папки и цель
BUILD_DIR = build
TARGET = $(BUILD_DIR)/email_client
SRC_DIR = src

# Компилятор и его настройки
CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++23 -I./$(SRC_DIR) -MMD -MP

# Флаги путей компоновщика 
LDFLAGS = 
# Сами библиотеки (СТРОГО LDLIBS, чтобы они встали в конец)
LDLIBS = -lsqlite3 -lyaml-cpp -lglog

# Поиск файлов
SRCS = $(shell find $(SRC_DIR) -name '*.cpp')
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

# Правила сборки
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Очистка сборки
clean:
	rm -rf $(BUILD_DIR)

# Подключение зависимостей заголовочных файлов
-include $(DEPS)

.PHONY: all clean