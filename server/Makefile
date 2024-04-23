BUILD_DIR := build

$(BUILD_DIR)/Server: $(BUILD_DIR)/main.o $(BUILD_DIR)/udp_server.o
	g++ -o $(BUILD_DIR)/Server $(BUILD_DIR)/main.o $(BUILD_DIR)/udp_server.o

$(BUILD_DIR)/main.o: main.cpp udp_server.h | $(BUILD_DIR)
	g++ -c main.cpp -I. -o $(BUILD_DIR)/main.o

$(BUILD_DIR)/udp_server.o: udp_server.cpp udp_server.h thread_pool.h TunInterface.h Authenticator.h | $(BUILD_DIR)
	g++ -c udp_server.cpp -I. -o $(BUILD_DIR)/udp_server.o

clean:
	rm -rf $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
