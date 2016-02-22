#include <iostream>
#include <fstream>
#include <stdio.h> // Needed for wren memory management
#include "game.hpp"
#include "window/sdl/sdlwindow.hpp"
#include "graphics/gl/glrenderer.hpp"
#include "error.hpp"
using namespace std;

#define DEFAULT_WINDOW_TITLE "Eigenspace"
#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600

void buttonDown(MouseButton button, int x, int y) {
  cout << button << " " << x << " " << y << endl;
}

void keyDown(string &key) {
  cout << key << endl;
}

static char *loadFile(const char *name) {
  string path = "./data/scripts/" + string(name);
  FILE *f = fopen(path.c_str(), "r");
  if(f == nullptr) {
    return nullptr;
  }
  fseek(f,0,SEEK_END);
  long fileSize = ftell(f);
  fseek(f,0,SEEK_SET);

  char *data = (char*)malloc(fileSize+1);
  fread(data,fileSize,1,f);
  fclose(f);

  data[fileSize] = 0;

  return data;
}

static char *loadModule(WrenVM *vm, const char *name) {
  return loadFile(name);
}

static void logText(WrenVM *vm, const char *text) {
  cout << text;
}

Game::Game() : 
  window(new SDLWindow(DEFAULT_WINDOW_TITLE)),
  renderer(new GLRenderer),
  input(new SDLInput)
{
  running = true;

  settings.windowTitle = DEFAULT_WINDOW_TITLE;
  settings.windowWidth = DEFAULT_WINDOW_WIDTH;
  settings.windowHeight = DEFAULT_WINDOW_HEIGHT;

  input->setMouseButtonDownCallback(buttonDown);
  input->setKeyDownCallback(keyDown);
  input->setResizeCallback(bind(&Game::windowResize, this, placeholders::_1, placeholders::_2));

  renderer->setRenderDimensions(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);

  initWren();
}

void Game::run() {
  Box box(renderer->getBlankTexture(), glm::vec2(100,100),glm::vec2(100,100),glm::vec4(1.0,0.0,0.0,1.0));
  while(running) {
    input->processInput();
    if(input->getQuit()) {
      running = false;
    }

    renderer->clearColour();
    renderer->clearDepth();
    renderer->drawBox(box);

    window->display();
  }
}

void Game::applySettings() {
  window->setResolution(settings.windowWidth, settings.windowHeight);
  window->setTitle(settings.windowTitle);
}

void Game::windowResize(int width, int height) {
  renderer->setRenderDimensions(width, height);
}

void Game::initWren() {
  WrenConfiguration config;
  wrenInitConfiguration(&config);

  config.loadModuleFn = loadModule;
  config.writeFn = logText;
  vm = wrenNewVM(&config);

  char *initData = loadFile("core.wren");
  if(initData == nullptr) {
    throw Error("Wren Init", "Could not open core.wren");
  }
  wrenInterpret(vm, initData);
  free(initData);
}