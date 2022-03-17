#include "ui/imgui_impl_sdl.h"
#include <CppLinuxSerial/SerialPort.hpp>
#include <SDL2/SDL.h>

#include "./timer.h"
#include "macros.h"
#include "ui/ui.h"
#include <cmath>
#include <iostream>
#include <thread>
#include <vector>
#if !SDL_VERSION_ATLEAST(2, 0, 17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

#define WINDOW_TITLE "SDL + Imgui"
unsigned int SCREEN_HEIGHT = 720;
unsigned int SCREEN_WIDTH = 1280;
float coloreSfondo[4] = {0, 0, 0, 1};
bool pause = true;

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;

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

using namespace mn::CppLinuxSerial;

void serialPortThread(SerialPort &sp, std::string &data) {
  std::string currentData;
  sp.SetTimeout(-1); // Block when reading until any data is received
  sp.Open();

  // Write some ASCII data
  sp.Write("Hello");
  while (pause) {
    sp.Read(currentData);
    if (currentData.length() > 0) {
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

  // ---------------------------------------------------------------------------

  // Create serial port object and open serial port at 57600 buad, 8 data bits,
  // no parity bit, and one stop bit (8n1)
  SerialPort serialPort("/dev/pts/4", BaudRate::B_57600, NumDataBits::EIGHT,
                        Parity::NONE, NumStopBits::ONE);
  // Use SerialPort serialPort("/dev/ttyACM0", 13000); instead if you want to
  // provide a custom baud rate

  // Read some data back (will block until at least 1 byte is received due to
  // the SetTimeout(-1) call above)

  auto *serialThread = new std::thread(serialPortThread, std::ref(serialPort),
                           std::ref(readData));

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

    ImGui::Begin("Data");
    char buf[100];
    ImGui::InputText("##", buf, 100);
    ImGui::SameLine();
    if(ImGui::Button("Send")) {
      serialPort.Write(buf);
    }
    if (ImGui::Checkbox("Continua", &pause) && pause) {
      std::cout << "resuming execution\n";
      serialPort.Close();
      serialThread->join();
      serialThread = new std::thread(serialPortThread, std::ref(serialPort),
                                 std::ref(readData));
    }
    ImGui::Text(readData.c_str());
    ImGui::End();

    ImGui::Render();

    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

    /* NOT YET SUPPORTED TODO: update when available
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
    }
    */
    SDL_RenderPresent(gRenderer);
  }
  pause = false;
  serialPort.Close();
  serialThread->join();

  ImGui_ImplSDLRenderer_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_DestroyRenderer(gRenderer);
  SDL_DestroyWindow(gWindow);

  SDL_Quit();
  return 0;
}
