#include "ui/imgui_impl_sdl.h"
#include <CppLinuxSerial/SerialPort.hpp>
#include <SDL2/SDL.h>

#include "./timer.h"
#include "macros.h"
#include "ui/ui.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#if !SDL_VERSION_ATLEAST(2, 0, 17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

#define WINDOW_TITLE "SDL + Imgui"
unsigned int SCREEN_HEIGHT = 720;
unsigned int SCREEN_WIDTH = 1280;
float coloreSfondo[4] = {0, 0, 0, 1};
bool started = false;

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;

int64_t msArrived = 0;
int64_t msSent = 0;

bool init() {
  bool success = true;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not be initialized, error: %s\n", SDL_GetError());
    success = false;
  } else {

    // THIS MAKES SURE COMPOSITOR IS NOT DISABLED AUTOMATICALLY ON LINUX
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
    SDL_WindowFlags window_flags =
        (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                          SDL_WINDOW_ALLOW_HIGHDPI);

    gWindow = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                               SCREEN_HEIGHT, window_flags);
    if (gWindow == NULL) {
      printf("Window could not be initialized, error: %s\n", SDL_GetError());
      success = false;
    } else {
      gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
      if (gRenderer == NULL) {
        printf("Renderer could not be initialized, error: "
               "%s\n",
               SDL_GetError());
      } else {
        SDL_RendererInfo info;
        SDL_GetRendererInfo(gRenderer, &info);
        SDL_Log("Current SDL_Renderer: %s", info.name);

        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
      }
    }
  }
  return success;
}

using std::cout;
namespace fs = std::filesystem;

std::vector<std::string> get_available_ports() {
  std::vector<std::string> port_names;

  const fs::path p("/dev/serial/by-id");
  try {
    if (!exists(p)) {
      throw std::runtime_error(p.generic_string() + " does not exist");
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

void serialPortThread(SerialPort &sp, std::string &data) {
  std::string currentData;

  while (started) {
    sp.Read(currentData);
    if (currentData.length() > 0) {
      msArrived = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::system_clock::now().time_since_epoch())
                      .count();
      data = currentData;
    }
  }
}

int main(int argc, char **argv) {
  if (!init()) {
    std::cout << "SDL Init Failed" << std::endl;
    return 1;
  }
  bool quit = false;
  SDL_Event e;
  Timer stepTimer;
  int x, y;
  std::string readData;

  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  io.WantCaptureKeyboard = true;
  io.Fonts->AddFontFromMemoryCompressedTTF(robotoFont_compressed_data,
                                           robotoFont_compressed_size, 16);
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.IniFilename = NULL;

  /** NOT YET SUPPORTED TODO: update when available
   * io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
   */

  ImGui_ImplSDL2_InitForSDLRenderer(gWindow, gRenderer);
  ImGui_ImplSDLRenderer_Init(gRenderer);

  std::vector<std::string> porteSeriali = get_available_ports();
  std::string porteDisponibiliString = "";
  for (auto porta : porteSeriali) {
    porteDisponibiliString += porta + "\n";
    std::cout << porta << std::endl;
  }

  // ---------------------------------------------------------------------------

  // Create serial port object and open serial port at 57600 buad, 8 data bits,
  // no parity bit, and one stop bit (8n1)
  SerialPort serialPort("/dev/ttyUSB0", BaudRate::B_57600, NumDataBits::EIGHT,
                        Parity::NONE, NumStopBits::ONE);
  serialPort.SetTimeout(0);

  // Use SerialPort serialPort("/dev/ttyACM0", 13000); instead if you want to
  // provide a custom baud rate

  // Read some data back (will block until at least 1 byte is received due to
  // the SetTimeout(-1) call above)

  std::thread *serialThread;

  // Close the serial port

  while (!quit) {
    int wheel = 0;
    SDL_GetMouseState(&x, &y);
    while (SDL_PollEvent(&e) != 0) {
      ImGui_ImplSDL2_ProcessEvent(&e);
      if (e.type == SDL_QUIT) {
        quit = true;
      } else if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_UP:
          std::cout << "up\n";
          break;
        case SDLK_DOWN:
          std::cout << "down\n";
          break;
        }
      } else if (e.type == SDL_WINDOWEVENT) {
        if (e.window.event == SDL_WINDOWEVENT_CLOSE &&
            e.window.windowID == SDL_GetWindowID(gWindow))
          quit = true;
        if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
          SDL_SetWindowSize(gWindow, e.window.data1, e.window.data2);
          SCREEN_WIDTH = e.window.data1;
          SCREEN_HEIGHT = e.window.data2;

          // TODO: resizing window fix for imgui
          // ImGuiSDL::Deinitialize();
          // ImGuiSDL::Initialize(gRenderer, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
      }
    }

    // Finestra impostazioni

    // --------------------
    // 	IMGUI
    // --------------------

    // ImGUI input handling

    const int buttons = SDL_GetMouseState(&x, &y);
    io.DeltaTime = 1.0f / 1000.0f;
    io.MousePos = ImVec2(static_cast<float>(x), static_cast<float>(y));
    io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
    io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
    io.MouseWheel = static_cast<float>(wheel);

    SDL_SetRenderDrawColor(gRenderer, COLORE(coloreSfondo));
    SDL_RenderClear(gRenderer);

    // float dt = stepTimer.getTicks() / 1000.f;

    stepTimer.start();

    // Rendering IMGUI
    renderUi(gWindow);

    ImGui::SetNextWindowDockID(main_dock, ImGuiCond_FirstUseEver);
    ImGui::Begin("Data");
    ImGui::Text("Available interfaces: \n%s", porteDisponibiliString.c_str());

    if (started) {
      if (ImGui::Button("T")) {
        msSent = std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::system_clock::now().time_since_epoch())
                     .count();
        serialPort.Write("t");
      }
      ImGui::SameLine();
      if (ImGui::Button("X")) {
        msSent = std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::system_clock::now().time_since_epoch())
                     .count();
        serialPort.Write("x");
      }
      if (ImGui::Button("Stop")) {
        std::cout << "Stopping execution\n";
        started = false;
        serialThread->join();
        serialPort.Close();
      }
    } else {
      if (ImGui::Button("Start")) {
        serialPort.Open();
        std::cout << "starting execution\n";
        started = true;
        serialThread = new std::thread(serialPortThread, std::ref(serialPort),
                                       std::ref(readData));
      }
    }

    ImGui::Text(readData.c_str());
    ImGui::Text("Average ping time: %fms", (msArrived - msSent) / 2.0f);

    ImGui::End();

    ImGui::Render();

    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(gRenderer);
  }
  if (started) {
    started = false;
    serialThread->join();
    serialPort.Close();
  }

  ImGui_ImplSDLRenderer_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_DestroyRenderer(gRenderer);
  SDL_DestroyWindow(gWindow);

  SDL_Quit();
  return 0;
}
