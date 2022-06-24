#include "interfaces.hpp"
#include "ui/imgui.h"
#include <CppLinuxSerial/SerialPort.hpp>
#include <algorithm>
#include <filesystem>
#include <iostream>

std::vector<std::string> SerialInterfaceManager::porteSeriali;
Interface SerialInterfaceManager::currentInterface;
uint8_t SerialInterfaceManager::nAvailableInterfaces;
mn::CppLinuxSerial::SerialPort
    *SerialInterfaceManager::serialPortCommunication = nullptr;

void getAvailablePorts(std::vector<std::string> &porte) {
  const std::filesystem::path p("/dev/serial/by-id");
  try {
    if (!exists(p)) {
      // throw std::runtime_error(p.generic_string() + " does not exist");
      std::cout << "No serial ports found\n";
    } else {
      for (std::filesystem::directory_entry de :
           std::filesystem::directory_iterator(p)) {
        if (is_symlink(de.symlink_status())) {
          std::filesystem::path symlink_points_at = read_symlink(de);
          std::filesystem::path canonical_path =
              std::filesystem::canonical(p / symlink_points_at);
          porte.push_back(canonical_path.generic_string());
        }
      }
    }
  } catch (const std::filesystem::filesystem_error &ex) {
    std::cout << ex.what() << '\n';
    throw ex;
  }
  std::sort(porte.begin(), porte.end());
}

SerialInterfaceManager::SerialInterfaceManager(
    mn::CppLinuxSerial::SerialPort *serialPort) {
  serialPortCommunication = serialPort;
  getAvailablePorts(porteSeriali);
  if (porteSeriali.size() > 0) {
    currentInterface.device = porteSeriali[0];
    currentInterface.index = 0;
    nAvailableInterfaces = porteSeriali.size();
    serialPortCommunication->SetDevice(currentInterface.device);
  }
}

size_t SerialInterfaceManager::getInterfacesCount() {
  return nAvailableInterfaces;
}

void SerialInterfaceManager::refreshInterfaces() {
  porteSeriali.clear();
  getAvailablePorts(porteSeriali);
  if (porteSeriali.size() > 0) {
    currentInterface.device = porteSeriali[0];
    currentInterface.index = 0;
    nAvailableInterfaces = porteSeriali.size();
    serialPortCommunication->SetDevice(currentInterface.device);
  } else {
          nAvailableInterfaces = 0;
    currentInterface.device = "";
  }
}

SerialInterfaceManager::~SerialInterfaceManager() { porteSeriali.clear(); }

void SerialInterfaceManager::renderCombo() {
  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
  if (nAvailableInterfaces < 1) {
    ImGui::BeginDisabled();
  }
  if (ImGui::BeginCombo("Interface", currentInterface.device.c_str())) {
    for (size_t i = 0; i < porteSeriali.size(); i++) {
      const bool is_selected = (currentInterface.index == i);
      if (ImGui::Selectable(porteSeriali[i].c_str(), is_selected)) {
        currentInterface.index = i;
        currentInterface.device = porteSeriali[i];
        serialPortCommunication->SetDevice(currentInterface.device);
      }

      // Set the initial focus when opening the combo (scrolling + keyboard
      // navigation focus)
      if (is_selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  if (nAvailableInterfaces > 0) {
    ImGui::Text("Current interface: %s", currentInterface.device.c_str());
  } else {
    ImGui::EndDisabled();
  }
}

Interface SerialInterfaceManager::getCurrentInterface() {
  return currentInterface;
}