#include "net.h"
#include "sim.h"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

#include <signal.h>
#include <string.h>
#include <chrono>
#include <thread>

#include "simulator_system.h"

SimulatorSystem *sys = NULL;

/*
 * Catch process signal to close the simulator using the right way
 */
void sig_handler(int sig);

/*
 * Execute self test before running core functions
 * If test fails the program returns
 */
bool execute_self_test();

int main(int argc, char **argv) {

  // Catch SIGTERM signal
  signal(SIGINT, sig_handler);
  signal(SIGTERM, sig_handler);
  signal(SIGKILL, sig_handler);
  signal(SIGABRT, sig_handler);

  /*
   * Initialize Verilator
   */
  Verilated::commandArgs(argc, argv);
  Verilated::debug(0);
#if TRACE
  Verilated::traceEverOn(true);
#endif

  /*
   * Initialize logger
   */
  auto file_logger = spdlog::basic_logger_mt("basic_logger", "logs/run.log");
  file_logger->flush_on(spdlog::level::info);
  spdlog::set_default_logger(file_logger);
  spdlog::info("Starting self test...");
  spdlog::info("Author : Corteggiani Nassim");
  spdlog::info("Contact: corteggi@eurecom.fr");
  spdlog::info("Company: EURECOM");

  // test the hardware model
  spdlog::info("Starting self test...");
  if (!execute_self_test() )
    spdlog::error("Self test failed...");
  else
    spdlog::info("Self test ok!");

  /*
   * TODO: To handle burst accesses, the simulator has a FIFO where commands are
   * stored So that, the external interface can always accept commands while the
   * FIFO is no full.
   */

  // Instantiate System
  sys = new SimulatorSystem();

  // Here you can relplace MKFifoNet by any class that inherits from AbstractNet
  // For instance you could use UDP or TCP socket instead depending on what you
  // matter of. It seems that mkfifo have less latency and highest throughput
  // than socket. But please check CRIU limitation first!
  AbstractNet *net = new MKFifo();

  AbstractSimulator *simulator = new AXISimulatorDriver();

  // System is compiled statically and linked with this program It avoids
  // recompiling all the stuff and having many times the same files in each
  // project. To connect components together we use the system class This class
  // will also manage the execution and turn components off when closing the
  // program
  if (!sys->set_network(net)) {
    spdlog::error("Unable to init network interface");
    return 0;
  }
  spdlog::info("Network inteface is up.");

  if (!sys->set_simulator(simulator)) {
    spdlog::error("Unable to init simulator");
    return 0;
  }
  spdlog::info("Simulator is up.");


  if (!sys->append_target(0, 0x43C10000, 0xFF)) {
    spdlog::error("Unable to init simulator");
    return 0;
  }
  spdlog::info("Adding target simulator.");

  if (!sys->append_target(1, 0x43C30000, 0xFF)) {
    spdlog::error("Unable to init simulator");
    return 0;
  }
  spdlog::info("Adding target simulator.");

  if (!sys->append_target(2, 0x43C20000, 0xFF)) {
    spdlog::error("Unable to init simulator");
    return 0;
  }
  spdlog::info("Adding target simulator.");

  // Blocking function
  spdlog::info("Running system components.");
  sys->run();

  spdlog::info("All components are down! Good Bye!");
}

bool execute_self_test() {

  AXISimulatorDriver *sim = new AXISimulatorDriver();
  sim->init();
  sim->select(0);
  sim->shutdown();
  std::cout << "[TEST] Success" << std::endl;
  return true;
}

void sig_handler(int sig) {

  spdlog::info("Shutting down components...");
  if (sys != NULL)
    sys->shutdown();
  spdlog::info("Components are done");

  //std::this_thread::sleep_for (std::chrono::seconds(10));

  exit(1);
}
