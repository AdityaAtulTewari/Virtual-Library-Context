#include <thread>
#include <iostream>
#include <vector>
#include <numeric>

#include "VLC/runtime.h"
#include "VLC/loader.h"

std::chrono::_V2::system_clock::time_point vlc_init_start;
std::chrono::_V2::system_clock::time_point vlc_init_end;

typedef int (*matvec_t)(int argc, char *argv[]);

void register_functions() {
    std::unordered_map<std::string, std::string> names{
        {"matvec", "_Z11kokkos_initi"}};
    VLC::Loader::register_func_names(names);
}

void launch(int vlc_id, int argc, char *argv[]) {
    VLC::Context vlc(vlc_id, gettid());
    vlc.avaliable_cpu("0-23"); // please change the number based on your system
    VLC::register_vlc(&vlc);
    VLC::Loader loader("libkokkos_compute.so", vlc_id, false);
    auto matvec = loader.load_func<matvec_t>("matvec");

    matvec(argc, argv);
}

int main(int argc, char *argv[]) {
    vlc_init_start = std::chrono::high_resolution_clock::now();
    VLC::Runtime vlc;     // initialize VLC environment
    vlc.initialize();
    register_functions();     // register functions used in VLC
    std::thread t;
    t = std::thread(launch, 1, argc, argv);
    t.join();
}
