#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#define MODE_EDITOR 0
#define MODE_ENGINE 1

class Clock;
class Log;
class Window;
class ResourceManager;
class InputManager;
class GUIManager;
class Renderer;
class Client;

class Environment {
public:
	Environment();
	~Environment();

	void set_mode(int mode);
	void set_clock(Clock* clock);
	void set_log(Log* log);
	void set_window(Window* window);
	void set_resource_manager(ResourceManager* resource_manager);
	void set_input_manager(InputManager* input_manager);
	void set_gui_manager(GUIManager* gui_manager);
	void set_renderer(Renderer* renderer);
	void set_client(Client* client);

	int     get_mode();
	Clock*  get_clock();
	Log*    get_log();
	Window* get_window();
	ResourceManager* get_resource_manager();
	InputManager* get_input_manager();
	GUIManager* get_gui_manager();
	Renderer* get_renderer();
	Client* get_client();

	void shut_down();

	static Environment& get();
private:
	int     _mode;
	Clock*  _clock;
	Log*    _log;
	Window* _window;
	ResourceManager* _resource_manager;
	InputManager* _input_manager;
	GUIManager* _gui_manager;
	Renderer* _renderer;
	Client* _client;

	static Environment* _instance;
};

#endif