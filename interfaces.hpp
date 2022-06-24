#pragma once
#include <vector>
#include <string>
#include <CppLinuxSerial/SerialPort.hpp>

std::vector<std::string> get_available_ports();

struct Interface {
    std::string device;
    uint8_t index;
};

class SerialInterfaceManager {
private:
  static std::vector<std::string> porteSeriali;
  static Interface currentInterface;
  static uint8_t nAvailableInterfaces;
  static mn::CppLinuxSerial::SerialPort *serialPortCommunication;

public:
  SerialInterfaceManager(mn::CppLinuxSerial::SerialPort *serialPort);

  size_t getInterfacesCount();

  void refreshInterfaces();

  ~SerialInterfaceManager();

  Interface getCurrentInterface();

  void renderCombo();
};