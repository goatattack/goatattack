#include "MasterServer.hpp"

int main(int argc, char *argv[]) {
    MasterServer master(25112, 25113);
    master.run();

    return 0;
}
