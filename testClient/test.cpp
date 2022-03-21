#include <CppLinuxSerial/SerialPort.hpp>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

using std::cout;
namespace fs = std::filesystem;
std::vector<std::string> get_available_ports() {
  std::vector<std::string> port_names;

  const fs::path p("/dev/serial/by-id");
  try {
    if (!exists(p)) {
      // throw std::runtime_error(p.generic_string() + " does not exist");
      std::cout << "No serial devices automatically detected.\n";
    } else {
      for (fs::directory_entry de : fs::directory_iterator(p)) {
        if (is_symlink(de.symlink_status())) {
          fs::path symlink_points_at = read_symlink(de);
          fs::path canonical_path = fs::canonical(p / symlink_points_at);
          port_names.push_back(canonical_path.generic_string());
        }
      }
    }
  } catch (const fs::filesystem_error &ex) {
    cout << ex.what() << '\n';
    throw ex;
  }
  std::sort(port_names.begin(), port_names.end());
  return port_names;
}

using namespace mn::CppLinuxSerial;

int main() {
  std::vector<std::string> porteSeriali = get_available_ports();
  std::string porteDisponibiliString = "";
  for (auto porta : porteSeriali) {
    porteDisponibiliString += porta + "\n";
    std::cout << porta << std::endl;
  }
  std::string portaCustom = "";
  std::cout << "Inserisci la porta da usare: ";
  std::cin >> portaCustom;
  SerialPort serialPort(portaCustom.c_str(), BaudRate::B_57600, NumDataBits::EIGHT,
                        Parity::NONE, NumStopBits::ONE);
  serialPort.SetTimeout(0);
  serialPort.Open();
  std::cout << "Connection opened to " << portaCustom << std::endl;

  std::cout << porteDisponibiliString << std::endl;

  std::vector<u_int8_t> currentData;
  while (true) {
    if(serialPort.Available() > 0)
      serialPort.ReadBinary(currentData);
    if (currentData.size() > 0) {
      std::cout << (int)currentData[0] << std::endl;
      currentData.clear();
/*       if (currentData == "t") {
        // int64_t ms =
        // std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        // std::cout << ms << std::endl;
        serialPort.Write("y");

      } else {
        serialPort.Write("n");
      } */
    }
  }
  serialPort.Close();
}
